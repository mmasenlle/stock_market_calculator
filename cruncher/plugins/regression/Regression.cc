
#include <list>
#include <map>
#include <float.h>
#include <math.h>
#include "logger.h"
#include "utils.h"
#include "matrix.h"
#include "equation.h"
#include "ICEvent.h"
#include "DBCache.h"
#include "CruncherConfig.h"
#include "Regression.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Regression;
}

Regression::Regression() : dbfeeder(&db), dbstatistics(&db), dbinterpolator(&db)
{
	state = CRUNCHER_RUNNING;
	stcs_updates = 1;
	int r = pthread_mutex_init(&mtx, NULL);
	if (r != 0 || (r = pthread_cond_init(&cond, NULL)) != 0)
	{
		ELOG("Regression::Regression() -> pthread_{mutex,cond}_init(): %d", r);
	}
	force_until = 0;
}

Regression::~Regression()
{
	int r = pthread_cond_destroy(&cond);
	if (r != 0 || (r = pthread_mutex_destroy(&mtx)) != 0)
	{
		ELOG("Regression::~Regression() -> pthread_{mutex,cond}_destroy(): %d", r);
	}
}

int Regression::init(ICruncherManager *icm)
{
	manager = icm;
	if (db.connect(manager->ccfg->db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Regression::init() -> db.connect(%s)", manager->ccfg->db_conninfo.c_str());
		return -1;
	}
	force_until = manager->ccfg->force_until;
	return 0;
}

// model:
// y(j) = a1 + a_x1(j-1) + a_x1(j-1)^2 + ... + a_x1(j-1)^r + 
//           + a_x2(j-1) + a_x2(j-1)^2 + ... + a_x2(j-1)^r + ...
//     ...   + a_xm(j-1) + a_xm(j-1)^2 + ... + a_xm(j-1)^r +
//           + a_x1(j-2) + a_x1(j-2)^2 + ... + a_x1(j-2)^r + 
//           + a_x2(j-2) + a_x2(j-2)^2 + ... + a_x2(j-2)^r + ...
//     ...   + a_xm(j-2) + a_xm(j-2)^2 + ... + a_xm(j-2)^r + ...
//     ...   + a_x1(j-k) + a_x1(j-k)^2 + ... + a_x1(j-k)^r + 
//           + a_x2(j-k) + a_x2(j-k)^2 + ... + a_x2(j-k)^r +
//     ...   + a_xm(j-k) + a_xm(j-k)^2 + ... + anxm(j-k)^r

#define INTERPM_R 3
#define INTERPM_M 4 //5
#define INTERPM_K 4 //5

struct x_t
{
	double x[INTERPM_M];
};

static void set_x(double *x, double *v)
{
	for (int k = 0; k < INTERPM_M; k++)
	{
		double xr = x[k];
		for (int l = 0; l < INTERPM_R; l++)
		{
			*v++ = xr;
			xr *= x[k];
		}
	}
}

void Regression::calculate(const char *cod, int start)
{
	DLOG("Regression::calculate(%s, %d)", cod, start);
	int empty_days = 0;
	int day_tail = start;
	int n = 1 + (INTERPM_R * INTERPM_M * INTERPM_K);
	std::list<x_t> xx;
	std::list<int> empty_queue;
	for (int day = start; force_until < day; day = utils::dec_day(day))
	{
		while (xx.size() < (n + INTERPM_K))
		{
			x_t x;
			if (!manager->cache->statistics__get_day(&dbstatistics, cod,
			    	STATISTICS_ITEM_PRICE, STATISTICS_STC_MIN, day_tail, &x.x[0]) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod,
				    STATISTICS_ITEM_PRICE, STATISTICS_STC_MAX, day_tail, &x.x[1]) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod,
				    STATISTICS_ITEM_PRICE, STATISTICS_STC_CLOSE, day_tail, &x.x[2]) ||
//				!manager->cache->statistics__get_day(&dbstatistics, cod,
//				    STATISTICS_ITEM_VOLUME, STATISTICS_STC_MEAN, day_tail, &x.x[4]) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod,
				    STATISTICS_ITEM_VOLUME, STATISTICS_STC_CLOSE, day_tail, &x.x[3]))
			{
				empty_queue.push_back(day_tail);
				day_tail = utils::dec_day(day_tail);
				if (!force_until || day_tail < force_until) empty_days++;
				if (empty_days > 15)
				{
					DLOG("Regression::calculate(%s) -> %08d..%08d (%d,%d) breaking ...",
					    cod, day_tail, day, empty_days, xx.size());
					if (xx.size() == (n + INTERPM_K)) break;
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
		double *mem = new double[4*n + 3*n*n];
		double *bbm = mem;
		double *bbM = mem + n;
		double *aa = mem + 2*n;
		double *uu = mem + 3*n;
		double *A = mem + 4*n;
		double *L = A + n*n;
		double *U = L + n*n;

		uu[0] = 1.0;
		double *_aa = uu + 1;
		std::list<x_t>::iterator jj = xx.begin();
		for (int i = 0; jj != xx.end(); jj++, i++, _aa += (INTERPM_R * INTERPM_M))
		{
			if (i < n)
			{
				bbm[i] = jj->x[0];
				bbM[i] = jj->x[1];
			}
			if (i >= INTERPM_K)
			{
#define OVERLAP ((INTERPM_K - 1) * INTERPM_R * INTERPM_M)
				*_aa = 1.0;
				memcpy(_aa + 1, _aa - OVERLAP, OVERLAP * sizeof(*_aa));
				_aa += OVERLAP + 1;
			}
			set_x(jj->x, _aa);
		}
		xx.pop_front();

		matrix::lu(n, A, L, U);

		equation::linsolve(n, bbm, L, U, aa);
		dbinterpolator.insert_equation(cod, day, INTERPT_MIN5, 0, n, aa);
		dbinterpolator.insert_result(cod, day, INTERPT_MIN5, matrix::dot(n, aa, uu), day);

		equation::linsolve(n, bbM, L, U, aa);
		dbinterpolator.insert_equation(cod, day, INTERPT_MAX5, 0, n, aa);
		dbinterpolator.insert_result(cod, day, INTERPT_MAX5, matrix::dot(n, aa, uu), day);

		delete [] mem;

		ILOG("Regression::calculate(%s) -> insert %08d", cod, day);
		if (!force_until)
			return;
	}
}

int Regression::run()
{
	manager->observe(ICEVENT_TRENDS_UPDATED);
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
		}
		ILOG("Regression::run() -> done for now (%08d, trends_updates: %d)", today, stcs_updates);
	}
	return 0;
}

int Regression::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_STATISTICS_UPDATED)
	{
		pthread_mutex_lock(&mtx);
		stcs_updates++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Regression::msg() -> ICEVENT_STATISTICS_UPDATED");
	}
	return 0;
}

int Regression::get_state()
{
	return state;	
}
