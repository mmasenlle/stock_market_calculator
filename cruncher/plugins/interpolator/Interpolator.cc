
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
#include "Interpolator.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Interpolator;
}

Interpolator::Interpolator() : dbfeeder(&db), dbstatistics(&db), dbinterpolator(&db)
{
	state = CRUNCHER_RUNNING;
	stcs_updates = 1;
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

#define INTERPM_R 2
#define INTERPM_M 5
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

void Interpolator::calculate(const char *cod, int start)
{
	DLOG("Interpolator::calculate(%s, %d)", cod, start);
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
				!manager->cache->statistics__get_day(&dbstatistics, cod,
				    STATISTICS_ITEM_VOLUME, STATISTICS_STC_MEAN, day_tail, &x.x[3]) ||
				!manager->cache->statistics__get_day(&dbstatistics, cod,
				    STATISTICS_ITEM_VOLUME, STATISTICS_STC_CLOSE, day_tail, &x.x[4]))
			{
				empty_queue.push_back(day_tail);
				day_tail = utils::dec_day(day_tail);
				if (!force_until || day_tail < force_until) empty_days++;
				if (empty_days > 15)
				{
					DLOG("Interpolator::calculate(%s) -> %08d..%08d (%d,%d) breaking ...",
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
//DLOG("Interpolator::calculate(%s) -> got data, constructing matrix ...", cod);
		double *bbm = new double[n];
		double *bbM = new double[n];
		double *uu = new double[(n + 1) * n];
		double *A = uu + n;
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
//for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) DLOG("a%d,%d: %g", i, j, A[(i*n)+j]);
//DLOG("Interpolator::calculate(%s) -> lu ...", cod);
		double *L = new double[n*n];
		double *U = new double[n*n];
		matrix::lu(n, A, L, U);
//DLOG("Interpolator::calculate(%s) -> data arranged, solving ...", cod);
		double *aa = new double[n];
//		memset(aa, 0, n * sizeof(*aa));
		equation::linsolve(n, bbm, L, U, aa);

		
//		double e = equation::solve(n, bbm, A, aa);
//DLOG("Interpolator::calculate(%s) -> solve error: %g, evaluating on today ...", cod, e);
//DLOG("Interpolator::calculate(%s) -> solved, evaluating on today ...", cod);
		double y = matrix::dot(n, aa, uu);
DLOG("Interpolator::calculate(%s) -> guessed tomorrow's min: %g, evaluating on yesterday ...", cod, y);
y = matrix::dot(n, aa, A);
DLOG("Interpolator::calculate(%s) -> guessed today's min: %g, today's min: %g, error: %g", cod, y, bbm[0], fabs(y - bbm[0]));

//		dbinterpolator.insert_equation(cod, day, INTERPT_MIN5, e, n, aa);
//		dbinterpolator.insert_result(cod, day, INTERPT_MIN5, y, day);
//		e = equation::solve(n, bbM, A, aa);
//		y = matrix::dot(n, aa, uu);
//		dbinterpolator.insert_equation(cod, day, INTERPT_MAX5, e, n, aa);
//		dbinterpolator.insert_result(cod, day, INTERPT_MAX5, y, day);

		delete [] L;
		delete [] U;
		delete [] bbm;
		delete [] bbM;
		delete [] uu;
		delete [] aa;

		ILOG("Interpolator::calculate(%s) -> insert %08d", cod, day);

		if (!force_until)
			return;
	}
}

int Interpolator::run()
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
		ILOG("Interpolator::run() -> done for now (%08d, trends_updates: %d)", today, stcs_updates);
	}
	return 0;
}

int Interpolator::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_STATISTICS_UPDATED)
	{
		pthread_mutex_lock(&mtx);
		stcs_updates++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		DLOG("Interpolator::msg() -> ICEVENT_STATISTICS_UPDATED");
	}
	return 0;
}

int Interpolator::get_state()
{
	return state;	
}
