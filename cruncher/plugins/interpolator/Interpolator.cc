
#include <list>
#include <map>
#include <float.h>
#include <math.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent.h"
#include "DBCache.h"
#include "CruncherConfig.h"
#include "Interpolator.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Interpolator;
}

Interpolator::Interpolator() : dbfeeder(&db), dbtrends(&db), dbinterpolator(&db)
{
	state = CRUNCHER_RUNNING;
	trends_updates = 1;
	int r = pthread_mutex_init(&mtx, NULL);
	if (r != 0 || (r = pthread_cond_init(&cond, NULL)) != 0)
	{
		ELOG("Interpolator::Interpolator() -> pthread_{mutex,cond}_init(): %d", r);
	}
	force_until = 0;
}

Interpolator::~Interpolator()
{
	int r = pthread_cond_destroy(&cond);
	if (r != 0 || (r = pthread_mutex_destroy(&mtx)) != 0)
	{
		ELOG("Interpolator::~Interpolator() -> pthread_{mutex,cond}_destroy(): %d", r);
	}
}

int Interpolator::init(ICruncherManager *icm)
{
	manager = icm;
	if (db.connect(manager->ccfg->db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Interpolator::init() -> db.connect(%s)", manager->ccfg->db_conninfo.c_str());
		return -1;
	}
	force_until = manager->ccfg->force_until;
	return 0;
}

void Interpolator::resolve(int n, const double *yy, const double *X, double *aa)
{
	double X_1[n][n];
//X_1 = X inverted
	for (int i = 0; i < n; i++)
	{
		aa[i] = 0.0;
		for (int j = 0; j < n; j++)
			aa[i] += X_1[i][j] * yy[j];
	}
}

#define INTERPOLATOR_ORDER (NR_INTERPOLATOR - INTERPOLATOR_a0)

void Interpolator::calculate(const char *cod, int start)
{
	DLOG("Interpolator::calculate(%s, %d)", cod, start);
	int empty_days = 0;
	int day_tail = start;
	std::list<double> xx;
	std::list<int> empty_queue;
	for (int day = start; force_until < day; day = utils::dec_day(day))
	{
		while (xx.size() < (INTERPOLATOR_ORDER + 1))
		{
			double x;
			if (!manager->cache->dbtrends__get(&dbtrends, cod, TRENDS_P, day_tail, &x))
			{
				empty_queue.push_back(day_tail);
				day_tail = utils::dec_day(day_tail);
				if (force_until < day_tail && ++empty_days > 15)
				{
					DLOG("Interpolator::calculate(%s) -> %08d..%08d (%d,%d) breaking ...",
					    cod, day_tail, day, empty_days, xx.size());
					if (xx.size() == (INTERPOLATOR_ORDER + 1)) break;
					else return;
				}
				continue;
			}
			empty_days = 0;
			xx.push_back(x);
			day_tail = utils::dec_day(day_tail);
		}
		if (!empty_queue.empty() && empty_queue.front() == day)
		{
			empty_queue.pop_front();
			continue;
		}
		double yy[INTERPOLATOR_ORDER];
		double X[INTERPOLATOR_ORDER][INTERPOLATOR_ORDER];
		std::list<double>::iterator i = xx.begin();
		for (int j = 0; j < INTERPOLATOR_ORDER; i++, j++)
		{
			yy[j] = *i;
			X[j][0] = 1.0;
		}
		xx.pop_front();
		i = xx.begin();
		for (int j = 0; j < INTERPOLATOR_ORDER; i++, j++)
		{
			for (int k = 1; k < INTERPOLATOR_ORDER; k++)
			{
				X[j][k] = X[j][k - 1] * (*i);
			}
		}
		double aa[INTERPOLATOR_ORDER];
		resolve(INTERPOLATOR_ORDER, yy, (const double *)X, aa);
		bool the_same = true;
		for (int k = INTERPOLATOR_a0; the_same && k < NR_INTERPOLATOR; k++)
		{
			std::vector<double> d;
			dbinterpolator.get(cod, k, day, day, &d, NULL);
			the_same = (d.size() == 1 && utils::equald(aa[k - INTERPOLATOR_a0], d[0]));
		}
		if (the_same)
		{
			if (force_until) continue;
			DLOG("Interpolator::calculate(%s) -> %08d same data, returning ...", cod, day);
			return;
		}
		double y = aa[INTERPOLATOR_ORDER - 1] * yy[0];
		for (int j = 2; j <= INTERPOLATOR_ORDER; j++)
			y = aa[INTERPOLATOR_ORDER - j] + (yy[0] * y);
		ILOG("Interpolator::calculate(%s) -> insert %08d", cod, day);
		dbinterpolator.insert(cod, day, y, aa);
	}
}

int Interpolator::run()
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
		for (int i = 0; i < codes.size(); i++)
		{
			calculate(codes[i].c_str(), today);
		}
		ILOG("Interpolator::run() -> done for now (%08d, trends_updates: %d)", today, trends_updates);
	}
	return 0;
}

int Interpolator::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_TRENDS_UPDATED)
	{
		pthread_mutex_lock(&mtx);
		trends_updates++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Interpolator::msg() -> ICEVENT_TRENDS_UPDATED");
	}
	return 0;
}

int Interpolator::get_state()
{
	return state;	
}
