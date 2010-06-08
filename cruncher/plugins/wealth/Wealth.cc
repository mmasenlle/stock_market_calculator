
#include <list>
#include <map>
#include <float.h>
#include <math.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent.h"
#include "DBCache.h"
#include "CruncherConfig.h"
#include "Wealth.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Wealth;
}

Wealth::Wealth() : dbfeeder(&db), dbstatistics(&db), dbtrends(&db), dbwealth(&db)
{
	state = CRUNCHER_RUNNING;
	trends_updates = 1;
	int r = pthread_mutex_init(&mtx, NULL);
	if (r != 0 || (r = pthread_cond_init(&cond, NULL)) != 0)
	{
		ELOG("Wealth::Wealth() -> pthread_{mutex,cond}_init(): %d", r);
	}
	force_until = 0;
}

Wealth::~Wealth()
{
	int r = pthread_cond_destroy(&cond);
	if (r != 0 || (r = pthread_mutex_destroy(&mtx)) != 0)
	{
		ELOG("Wealth::~Wealth() -> pthread_{mutex,cond}_destroy(): %d", r);
	}
}

int Wealth::init(ICruncherManager *icm)
{
	manager = icm;
	if (db.connect(manager->ccfg->db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Wealth::init() -> db.connect(%s)", manager->ccfg->db_conninfo.c_str());
		return -1;
	}
	force_until = manager->ccfg->force_until;
	return 0;
}

int Wealth::run()
{
	manager->observe(ICEVENT_TRENDS_UPDATED);
	for (;;)
	{
		pthread_mutex_lock(&mtx);
		state = CRUNCHER_WAITING;
		while (!trends_updates)
			pthread_cond_wait(&cond, &mtx);
		state = CRUNCHER_RUNNING;
		trends_updates = 0;
		pthread_mutex_unlock(&mtx);

		int today = utils::today();
		std::vector<std::string> codes;
		manager->cache->feeder__get_value_codes(&dbfeeder, &codes);
		int empty_days = 0;
		for (int day = today; force_until < day; day = utils::dec_day(day))
		{
			DLOG("Wealth::run() -> day: %08d", day);
			double sday[LAST_STATISTICS_ITEM][LAST_STATISTICS_STC];
			memset(sday, 0, sizeof(sday));
			double trends[NR_TRENDS];
			memset(trends, 0, sizeof(trends));
			double trends_acum[NR_TRENDS_ACUM];
			memset(trends_acum, 0, sizeof(trends_acum));
			int count = 0;
			for (int i = 0; i < codes.size(); i++)
			{
				bool some_data = false;
				for (int j = 0; j < LAST_STATISTICS_ITEM; j++)
				{
					for (int k = 0; k < LAST_STATISTICS_STC; k++)
					{
						double v;
						if (manager->cache->statistics__get_day(&dbstatistics, codes[i].c_str(), j, k, day, &v))
						{
							some_data = true;
							sday[j][k] += v;
						}
					}
				}
				for (int j = 0; j < NR_TRENDS; j++)
				{
					double v;
					if (manager->cache->dbtrends__get(&dbtrends, codes[i].c_str(), j, day, &v))
					{
						some_data = true;
						trends[j] += v;
					}
				}
				for (int j = 0; j < NR_TRENDS_ACUM; j++)
				{
					std::vector<double> d;
					dbtrends.get_acum(codes[i].c_str(), j, day, day, &d, NULL);
					if (!d.empty())
					{
						some_data = true;
						trends_acum[j] += d.front();
					}
				}
				if (some_data) count++;
			}
			if (count)
			{
				empty_days = 0;
				bool all_the_same = true;
				bool the_same = true;
				for (int j = 0; the_same && j < LAST_STATISTICS_ITEM; j++)
				{
					for (int k = 0; the_same && k < LAST_STATISTICS_STC; k++)
					{
						std::vector<double> d;
						dbwealth.get_sday(j, k, day, day, &d, NULL);
						the_same = (d.size() == 1 && utils::equald(sday[j][k], d[0]));
					}
				}
				if (!the_same)
				{
					all_the_same = false;
					ILOG("Wealth::run() -> insert_sday %08d (%d)", day, count);
					dbwealth.insert_sday(day, sday[0][0], sday[1][0], sday[2][0],
					    sday[0][1], sday[1][1], sday[2][1], sday[0][2], sday[1][2], sday[2][2],
					    sday[0][3], sday[1][3], sday[2][3], sday[0][4], sday[1][4], sday[2][4],
					    sday[0][5], sday[1][5], sday[2][5], sday[0][6], sday[1][6], sday[2][6]);
				}
				the_same = true;
				for (int j = 0; j < NR_TRENDS; j++)
				{
					std::vector<double> d;
					dbwealth.get_trends(j, day, day, &d, NULL);
					the_same = (d.size() == 1 && utils::equald(trends[j], d[0]));
				}
				if (!the_same)
				{
					all_the_same = false;
					ILOG("Wealth::run() -> insert_trends %08d (%d)", day, count);
					dbwealth.insert_trends(day, trends[0], trends[1], trends[2], trends[3],
					    trends[4], trends[5], trends[6], trends[7], trends[8], trends[9]);
				}
				the_same = true;
				for (int j = 0; j < NR_TRENDS_ACUM; j++)
				{
					std::vector<double> d;
					dbwealth.get_trends_acum(j, day, day, &d, NULL);
					the_same = (d.size() == 1 && utils::equald(trends_acum[j], d[0]));
				}
				if (!the_same)
				{
					all_the_same = false;
					ILOG("Wealth::run() -> insert_trends_acum %08d (%d)", day, count);
					dbwealth.insert_trends_acum(day, trends_acum[0], trends_acum[1], trends_acum[2],
					    trends_acum[3], trends_acum[4], trends_acum[5], trends_acum[6]);
				}
				if (all_the_same && !force_until)
				{
					DLOG("Wealth::run() -> %08d same data, breaking ...", day);
					break;
				}
			}
			else if (!force_until && ++empty_days > 15)
			{
				DLOG("Wealth::run() -> %08d empty_days: %d, breaking ...", day, empty_days);
				break;
			}
		}
		ILOG("Wealth::run() -> done for now (%08d, trends_updates: %d)", today, trends_updates);
	}
	return 0;
}

int Wealth::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_TRENDS_UPDATED)
	{
		pthread_mutex_lock(&mtx);
		trends_updates++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Wealth::msg() -> ICEVENT_TRENDS_UPDATED");
	}
	return 0;
}

int Wealth::get_state()
{
	return state;	
}
