
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

Wealth::Wealth() : dbwealth(&db)
{
	stcs_updates = 1;
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
	manager->observe(ICEVENT_STATISTICS_UPDATED);
	for (;;)
	{
		pthread_mutex_lock(&mtx);
		while (!stcs_updates)
			pthread_cond_wait(&cond, &mtx);
		stcs_updates = 0;
		pthread_mutex_unlock(&mtx);

		int today = utils::today();
		std::vector<std::string> codes;
		manager->cache->feeder__get_value_codes(&codes);
		int empty_days = 0;
		for (int day = today; force_until < day; day = utils::dec_day(day))
		{
			DLOG("Wealth::run() -> day: %08d", day);
			int count = 0;
			double r[LAST_STATISTICS_ITEM][LAST_STATISTICS_STC];
			memset(r, 0, sizeof(r));
			for (int i = 0; i < codes.size(); i++)
			{
				bool some_data = false;
				for (int j = 0; j < LAST_STATISTICS_ITEM; j++)
				{
					for (int k = 0; k < LAST_STATISTICS_STC; k++)
					{
						double v;
						if (manager->cache->statistics__get_day(codes[i].c_str(), j, k, day, &v))
						{
							some_data = true;
							r[j][k] += v;
						}
					}
				}
				if (some_data) count++;
			}
			if (!count)
			{
				if (!force_until && ++empty_days > 15)
				{
					DLOG("Wealth::run() -> %08d empty_days: %d, breaking ...", day, empty_days);
					break;
				}
				continue;
			}
			empty_days = 0;
			bool the_same = true;
			for (int j = 0; the_same && j < LAST_STATISTICS_ITEM; j++)
			{
				for (int k = 0; the_same && k < LAST_STATISTICS_STC; k++)
				{
					std::vector<double> d;
					dbwealth.get(j, k, day, day, &d, NULL);
					the_same = (d.size() == 1 && utils::equald(r[j][k], d[0]));
				}
			}
			if (the_same)
			{
				if (force_until) continue;
				DLOG("Wealth::run() -> %08d same data, breaking ...", day);
				break;
			}
			ILOG("Wealth::run() -> insert %08d (%d)", day, count);
			dbwealth.insert(day, r[0][0], r[1][0], r[2][0],
					r[0][1], r[1][1], r[2][1], r[0][2], r[1][2], r[2][2],
					r[0][3], r[1][3], r[2][3], r[0][4], r[1][4], r[2][4],
					r[0][5], r[1][5], r[2][5], r[0][6], r[1][6], r[2][6]);
		}
		ILOG("Wealth::run() -> done for now (%08d, stcs_updates: %d)", today, stcs_updates);
	}
	return 0;
}

int Wealth::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_STATISTICS_UPDATED)
	{
		pthread_mutex_lock(&mtx);
		stcs_updates++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Wealth::msg() -> ICEVENT_STATISTICS_UPDATED");
	}
	return 0;
}
