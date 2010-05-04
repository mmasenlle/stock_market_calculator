
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
		std::vector<double> v;
		dbfeeder.get_value_data(cod, FEEDER_DATAITEM_PRICE, day_before, 0, day_before, 0, &v, NULL, NULL);
		if (v.empty())
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
		double C = v.back();
		for (int j = 0; j < v.size(); j++)
		{
			if (L > v.at(j)) L = v.at(j);
			if (H < v.at(j)) H = v.at(j);
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
				trends[3], trends[4], trends[5], trends[6], trends[7], trends[8]);
cont:
		day = day_before;
		day_before = utils::dec_day(day);
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
				dbfeeder.get_value_data(codes[i].c_str(), 0, today, 0, 0, 0,
				    NULL, &dates, NULL);
				if (dates.size())
				{
					recentst_day = dates.back();
					break;
				}
				if (!force_until && j > 25)
				{
					WLOG("Statistics::run(%s) -> can't find recentst day", codes[i].c_str());
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

			last_stamps[codes[i]] == recentst_day;
			DLOG("Statistics::run(%s) -> last_stamp %08d", codes[i].c_str(), recentst_day);
		}
		ILOG("Statistics::run() -> done for now (last_stamps.size(): %d, newfeeds: %d)",
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
