
#include <list>
#include <map>
#include <float.h>
#include <math.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent.h"
#include "CruncherConfig.h"
#include "Trends.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Trends;
}

Trends::Trends() : dbfeeder(&db), dbtrends(&db)
{
	newfeeds = 0;
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
	int day = start;
	int day_before = utils::dec_day(day);
	while (force_until < day_before)
	{
		std::vector<double> prices, volumes;
		dbfeeder.get_value_data(cod, FEEDER_DATAITEM_PRICE, day_before, 0, day_before, 0, &prices, NULL, NULL);
		dbfeeder.get_value_data(cod, FEEDER_DATAITEM_VOLUME, day_before, 0, day_before, 0, &volumes, NULL, NULL);
		if (prices.empty())
		{
			if (!force_until && ++empty_days > 15)
			{
				DLOG("Trends::calculate(%s) -> %08d empty_days: %d, returning ...", cod, day_before, empty_days);
				return;
			}
			day_before = utils::dec_day(day_before);
			continue;
		}
		empty_days = 0;
		double L = DBL_MAX;
		double H = -DBL_MAX;
		double C = prices.back();
		for (int j = 0; j < prices.size(); j++)
		{
			if (L > prices.at(j)) L = prices.at(j);
			if (H < prices.at(j)) H = prices.at(j);
		}
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
		trends[TRENDS_MF] = volumes.empty() ? .0 : (trends[TRENDS_P] * volumes.back());
		bool the_same = true;
		for (int k = 0; the_same && k < NR_TRENDS; k++)
		{
			std::vector<double> d;
			dbtrends.get(cod, k, day, day, &d, NULL);
			the_same = (d.size() == 1 && utils::equald(trends[k], d[0]));
		}
		if (the_same)
		{
			if (force_until) goto cont;
			DLOG("Trends::calculate(%s) -> %08d same data, returning ...", cod, day);
			return;
		}
		ILOG("Trends::calculate(%s) -> insert %08d", cod, day);
		dbtrends.insert(cod, day, trends[0], trends[1], trends[2],
				trends[3], trends[4], trends[5], trends[6], trends[7], trends[8], trends[9]);
cont:
		day = day_before;
		day_before = utils::dec_day(day);
	}
}

#define TRENDACUM_N 10

void Trends::calculate_acum(const char *cod, int start)
{
	DLOG("Trends::calculate_acum(%s, %d)", cod, start);
	int empty_days = 0;
	int day_tail = start;
	std::list<std::pair<double, double> > trend_tail;
	for (int day = start; force_until < day; day = utils::dec_day(day))
	{
		while (trend_tail.size() < TRENDACUM_N)
		{
			std::vector<double> P, MF;
			dbtrends.get(cod, TRENDS_P, day_tail, day_tail, &P, NULL);
			dbtrends.get(cod, TRENDS_MF, day_tail, day_tail, &MF, NULL);
			day_tail = utils::dec_day(day_tail);
			if (P.empty() || MF.empty())
			{
				if (force_until < day_tail && ++empty_days > 15)
				{
					DLOG("Trends::calculate_acum(%s) -> %08d empty_days: %d, breaking ...", cod, day_tail, empty_days);
					if (trend_tail.size() > 1) break;
					else return;
				}
				continue;
			}
			empty_days = 0;
			trend_tail.push_back(std::make_pair<double, double>(P.front(), MF.front()));
		}
		double trends_acum[NR_TRENDS_ACUM];
		memset(trends_acum, 0, sizeof(trends_acum));
		double pMF = 0.0, nMF = 0.0;
		std::list<std::pair<double, double> >::iterator j = trend_tail.end();
		for (std::list<std::pair<double, double> >::iterator i = trend_tail.begin(),
		    j = trend_tail.end(); i != trend_tail.end(); j = i++)
		{
			trends_acum[TRENDS_ACUM_SMA] += i->first;
			if (j != trend_tail.end())
			{
				if (j->first < i->first)
					pMF += i->second;
				else if (j->first > i->first)
					nMF += i->second;
			}
		}
		trends_acum[TRENDS_ACUM_SMA] /= trend_tail.size();
		for (std::list<std::pair<double, double> >::iterator i = trend_tail.begin();
		    i != trend_tail.end(); i++)
			trends_acum[TRENDS_ACUM_MAD] += fabs(i->first - trends_acum[TRENDS_ACUM_SMA]);
		trends_acum[TRENDS_ACUM_MAD] /= trend_tail.size();
		if (trends_acum[TRENDS_ACUM_MAD])
			trends_acum[TRENDS_ACUM_CCI] = 
				(trend_tail.front().first - trends_acum[TRENDS_ACUM_SMA]) /
				(.015 * trends_acum[TRENDS_ACUM_MAD]);
		if (trend_tail.back().first)
			trends_acum[TRENDS_ACUM_ROC] = 
				(trend_tail.front().first - trend_tail.back().first) / trend_tail.back().first;
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
		    trends_acum[3], trends_acum[4]);
	}
}

int Trends::run()
{
	manager->observe(ICEVENT_FEEDER_NEWFEED);
	std::map<std::string, int> last_stamps;
	
	for (;;)
	{
		int today = utils::today();
		std::vector<std::string> codes;
		dbfeeder.get_value_codes(&codes);
		for (int i = 0; i < codes.size(); i++)
		{
			int recentst_day = today;
			for (int j = 0; force_until < recentst_day; j++)
			{
				std::vector<int> dates;
				dbfeeder.get_value_data(codes[i].c_str(), 0, recentst_day, 0, 0, 0,
				    NULL, &dates, NULL);
				if (dates.size())
				{
					recentst_day = dates.back();
					break;
				}
				if (!force_until && j > 25)
				{
					WLOG("Trends::run(%s) -> can't find recentst day", codes[i].c_str());
					break;
				}	
				recentst_day = utils::dec_day(recentst_day);
			}
			if (last_stamps.find(codes[i]) != last_stamps.end())
			{
				if (last_stamps[codes[i]] == recentst_day)
					continue;
			}
			calculate(codes[i].c_str(), recentst_day);
			calculate_acum(codes[i].c_str(), recentst_day);

			last_stamps[codes[i]] == recentst_day;
			DLOG("Trends::run(%s) -> last_stamp %08d", codes[i].c_str(), recentst_day);
		}
		ILOG("Trends::run() -> done for now (last_stamps.size(): %d, newfeeds: %d)",
				last_stamps.size(), newfeeds);
		pthread_mutex_lock(&mtx);
		while (!newfeeds)
			pthread_cond_wait(&cond, &mtx);
		newfeeds = 0;
		pthread_mutex_unlock(&mtx);
	}

	return 0;
}

int Trends::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_FEEDER_NEWFEED)
	{
		pthread_mutex_lock(&mtx);
		newfeeds++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Trends::msg() -> ICEVENT_FEEDER_NEWFEED (%d)", newfeeds);
	}
	return 0;
}
