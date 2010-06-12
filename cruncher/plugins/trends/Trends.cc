
#include <list>
#include <map>
#include <float.h>
#include <math.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent.h"
#include "DBCache.h"
#include "CruncherConfig.h"
#include "Trends.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Trends;
}

Trends::Trends() : dbfeeder(&db), dbstatistics(&db), dbtrends(&db), trends_updated(ICEVENT_TRENDS_UPDATED)
{
	state = CRUNCHER_RUNNING;
	stcs_updates = 1;
	int r = pthread_mutex_init(&mtx, NULL);
	if (r != 0 || (r = pthread_cond_init(&cond, NULL)) != 0)
	{
		ELOG("Trends::Trends() -> pthread_{mutex,cond}_init(): %d", r);
	}
	force_until = 0;
}

Trends::~Trends()
{
	int r = pthread_cond_destroy(&cond);
	if (r != 0 || (r = pthread_mutex_destroy(&mtx)) != 0)
	{
		ELOG("Trends::~Trends() -> pthread_{mutex,cond}_destroy(): %d", r);
	}
}

int Trends::init(ICruncherManager *icm)
{
	manager = icm;
	if (db.connect(manager->ccfg->db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Trends::init() -> db.connect(%s)", manager->ccfg->db_conninfo.c_str());
		return -1;
	}
	force_until = manager->ccfg->force_until;
	return 0;
}

void Trends::calculate(const char *cod, int start)
{
	DLOG("Trends::calculate(%s, %d)", cod, start);
	int empty_days = 0;
	for (int day = start; force_until < day; day = utils::dec_day(day))
	{
		double L, H, C, V;
		if (!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_MIN, day, &L) ||
			!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_MAX, day, &H) ||
			!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_CLOSE, day, &C) ||
			!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_VOLUME, STATISTICS_STC_CLOSE, day, &V))
		{
			if (!force_until && ++empty_days > 15)
			{
				DLOG("Trends::calculate(%s) -> %08d empty_days: %d, returning ...", cod, day, empty_days);
				return;
			}
			continue;
		}
		empty_days = 0;
		double trends[NR_TRENDS];
		trends[TRENDS_P] = (L + H + C) / 3.0;
		trends[TRENDS_R1] = (2 * trends[TRENDS_P]) - L;
		trends[TRENDS_S1] = (2 * trends[TRENDS_P]) - H;
		trends[TRENDS_R2] = trends[TRENDS_P] + (H - L);
		trends[TRENDS_S2] = trends[TRENDS_P] - (H - L);
		trends[TRENDS_R3] = trends[TRENDS_R2] + (H - L);
		trends[TRENDS_S3] = trends[TRENDS_S2] - (H - L);
		trends[TRENDS_R4] = trends[TRENDS_R3] + (H - L);
		trends[TRENDS_S4] = trends[TRENDS_S3] - (H - L);
		trends[TRENDS_MF] = trends[TRENDS_P] * V;
		if (manager->cache->dbtrends__insert(&dbtrends, cod, day, trends))
		{
			ILOG("Trends::calculate(%s) -> insert %08d", cod, day);
		}
		else
		{
			if (force_until) continue;
			DLOG("Trends::calculate(%s) -> %08d same data, returning ...", cod, day);
			return;
		}
	}
}

#define TRENDACUM_N 10

struct trend_tail_t
{
	double L;
	double H;
	double C;
	double V;
	double P;
	double MF;
	trend_tail_t(double l, double h, double c, double v, double p, double mf)
	: L(l), H(h), C(c), V(v), P(p), MF(mf) {};
};

void Trends::calculate_acum(const char *cod, int start)
{
	DLOG("Trends::calculate_acum(%s, %d)", cod, start);
	int empty_days = 0;
	int day_tail = start;
	std::list<trend_tail_t> trend_tail;
	std::list<int> empty_queue;
	for (int day = start; force_until < day; day = utils::dec_day(day))
	{
		while (trend_tail.size() < TRENDACUM_N)
		{
			double L, H, C, V, P, MF;
			if (!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_MIN, day_tail, &L) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_MAX, day_tail, &H) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_CLOSE, day_tail, &C) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod, STATISTICS_ITEM_VOLUME, STATISTICS_STC_CLOSE, day_tail, &V) ||
				!manager->cache->dbtrends__get(&dbtrends, cod, TRENDS_P, day_tail, &P) ||
				!manager->cache->dbtrends__get(&dbtrends, cod, TRENDS_MF, day_tail, &MF))
			{
				empty_queue.push_back(day_tail);
				day_tail = utils::dec_day(day_tail);
				if (!force_until || day_tail < force_until) empty_days++;
				if (empty_days > 15)
				{
					DLOG("Trends::calculate_acum(%s) -> %08d..%08d (%d,%d) breaking ...",
					    cod, day_tail, day, empty_days, trend_tail.size());
					if (trend_tail.size() > 1) break;
					else return;
				}
				continue;
			}
			empty_days = 0;
			trend_tail.push_back(trend_tail_t(L, H, C, V, P, MF));
			day_tail = utils::dec_day(day_tail);
		}
		if (!empty_queue.empty() && empty_queue.front() == day)
		{
			empty_queue.pop_front();
			continue;
		}
		double trends_acum[NR_TRENDS_ACUM];
		memset(trends_acum, 0, sizeof(trends_acum));
		double pMF = 0.0, nMF = 0.0;
		std::list<trend_tail_t>::iterator j = trend_tail.end();
		for (std::list<trend_tail_t>::iterator i = trend_tail.begin(),
		    j = trend_tail.end(); i != trend_tail.end(); j = i++)
		{
			trends_acum[TRENDS_ACUM_SMA] += i->P;
			if (j != trend_tail.end())
			{
				if (j->P < i->P)
					pMF += i->MF;
				else if (j->P > i->P)
					nMF += i->MF;
				if (j->C < i->C)
					trends_acum[TRENDS_ACUM_OBV] += i->V;
				else if (j->C > i->C)
					trends_acum[TRENDS_ACUM_OBV] -= i->V;
			}
			if (i->L != i->H)
			{
				trends_acum[TRENDS_ACUM_AD] += 
					(((i->C - i->L) - (i->H - i->C)) * i->V) / (i->H - i->L);
			}
		}
		trends_acum[TRENDS_ACUM_SMA] /= trend_tail.size();
		for (std::list<trend_tail_t>::iterator i = trend_tail.begin();
		    i != trend_tail.end(); i++)
			trends_acum[TRENDS_ACUM_MAD] += fabs(i->P - trends_acum[TRENDS_ACUM_SMA]);
		trends_acum[TRENDS_ACUM_MAD] /= trend_tail.size();
		if (trends_acum[TRENDS_ACUM_MAD])
			trends_acum[TRENDS_ACUM_CCI] = 
				(trend_tail.front().P - trends_acum[TRENDS_ACUM_SMA]) /
				(.015 * trends_acum[TRENDS_ACUM_MAD]);
		if (trend_tail.back().P)
			trends_acum[TRENDS_ACUM_ROC] = 
				(trend_tail.front().P - trend_tail.back().P) / trend_tail.back().P;
		if (pMF) trends_acum[TRENDS_ACUM_MFI] = (100 * pMF) / (pMF + nMF);
		trend_tail.pop_front();
		bool the_same = true;
		for (int k = 0; the_same && k < NR_TRENDS_ACUM; k++)
		{
			std::vector<double> d;
			dbtrends.get_acum(cod, k, day, day, &d, NULL);
			the_same = (d.size() == 1 && utils::equald(trends_acum[k], d[0]));
		}
		if (the_same)
		{
			if (force_until) continue;
			DLOG("Trends::calculate_acum(%s) -> %08d same data, returning ...", cod, day);
			return;
		}
		ILOG("Trends::calculate_acum(%s) -> insert %08d (%d)", cod, day, trend_tail.size() + 1);
		dbtrends.insert_acum(cod, day, trends_acum[0], trends_acum[1], trends_acum[2],
		    trends_acum[3], trends_acum[4], trends_acum[5], trends_acum[6]);
	}
}

int Trends::run()
{
	manager->observe(ICEVENT_STATISTICS_UPDATED);
	for (;;)
	{
		pthread_mutex_lock(&mtx);
		state = CRUNCHER_WAITING;
		while (!stcs_updates)
			pthread_cond_wait(&cond, &mtx);
		state = CRUNCHER_RUNNING;
		stcs_updates = 0;
		pthread_mutex_unlock(&mtx);

		int today = utils::today();
		std::vector<std::string> codes;
		manager->cache->feeder__get_value_codes(&dbfeeder, &codes);
		for (int i = 0; i < codes.size(); i++)
		{
			calculate(codes[i].c_str(), today);
			calculate_acum(codes[i].c_str(), today);
		}
		manager->send(&trends_updated, NULL);
		ILOG("Trends::run() -> done for now (%08d, stcs_updates: %d)", today, stcs_updates);
	}
	return 0;
}

int Trends::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_STATISTICS_UPDATED)
	{
		pthread_mutex_lock(&mtx);
		stcs_updates++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Trends::msg() -> ICEVENT_STATISTICS_UPDATED");
	}
	return 0;
}

int Trends::get_state()
{
	return state;	
}
