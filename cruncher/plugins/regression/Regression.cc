
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
#define INTERPM_M 5
#define INTERPM_K 5

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
	std::vector<double> v[INTERPM_M];
	std::vector<int> d[INTERPM_M];
	dbstatistics.get_day(cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_MIN, 0, start, &v[0], &d[0]);
	dbstatistics.get_day(cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_MAX, 0, start, &v[1], &d[1]);
	dbstatistics.get_day(cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_CLOSE, 0, start, &v[2], &d[2]);
	dbstatistics.get_day(cod, STATISTICS_ITEM_PRICE, STATISTICS_STC_STD, 0, start, &v[3], &d[3]);
	dbstatistics.get_day(cod, STATISTICS_ITEM_VOLUME, STATISTICS_STC_CLOSE, 0, start, &v[4], &d[4]);	
	int n = 1 + (INTERPM_R * INTERPM_M * INTERPM_K);
	std::list<x_t> xx;
	std::list<int> empty_queue;
	bool bbreak = false;
	for (int day_tail = start; !bbreak; day_tail = utils::dec_day(day_tail))
	{
		x_t x;
		for (int i = 0; i < INTERPM_M; i++)
		{
			while (!d[i].empty() && d[i].back() > day_tail) 
				d[i].pop_back(), v[i].pop_back();
			if (d[i].empty())
			{
				DLOG("Regression::calculate(%s) -> %08d..%08d (%d) breaking ...",
					    cod, day_tail, start, xx.size());
				if (xx.size() >= (n + INTERPM_K))
				{
					bbreak = true;
					break;
				}
				else return;
			}
			if (d[i].back() != day_tail)
			{
				if (day_tail != start) empty_queue.push_back(day_tail);
				else start = utils::dec_day(start);
				break;
			}
			x.x[i] = v[i].back();
			if (i == 4)
			{
				x.x[4] = log(x.x[4]);
				xx.push_back(x);
			}
		}
	}
	int m = xx.size() - INTERPM_K;
	double *bbm = new double[m];
	double *bbM = new double[m];
	double *uu = new double[(m + 1) * n];
	double *A = uu + n;
	uu[0] = 1.0;
	double *_aa = uu + 1;
	std::list<x_t>::iterator jj = xx.begin();
	for (int i = 0; jj != xx.end(); jj++, i++, _aa += (INTERPM_R * INTERPM_M))
	{
		if (i < m)
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
	xx.clear();
	double *At = new double[n * m];
	matrix::transp(m, n, A, At);
	double *AtA = new double[n * n];
	double *Atbbm = new double[n];
	double *AtbbM = new double[n];
	matrix::mul(n, m, n, At, A, AtA);
	matrix::mul(n, m, 1, At, bbm, Atbbm);
	matrix::mul(n, m, 1, At, bbM, AtbbM);
	delete [] At;
	double *L = new double[n * n];
	double *U = new double[n * n];
	double *aam = new double[n];
	double *aaM = new double[n];
	double em = 0, eM = 0;
	if (!matrix::lu(n, AtA, L, U))
	{
		WLOG("Regression::calculate(%s, %d) -> matrix::lu() failed (%d, %d)", cod, start, m, n);
		goto cleanup;
	}
	equation::linsolve(n, Atbbm, L, U, aam);
	equation::linsolve(n, AtbbM, L, U, aaM);
	for (int i = 0; i < m; i++)
	{
		em += fabs(matrix::dot(n, aam, A + (n * i)) - bbm[i]);
		eM += fabs(matrix::dot(n, aaM, A + (n * i)) - bbM[i]);
	}
	DLOG("Regression::calculate(%s, %d) -> (m, n): %d, %d; em: %g, eM: %g", cod, start, m, n, em, eM);
	if (em != em || eM != eM)
	{
		WLOG("Regression::calculate(%s, %d) -> e == NaN", cod, start);
		goto cleanup;
	}
	dbinterpolator.insert_equation(cod, start, INTERPT_RMIN5, em, n, aam);
	dbinterpolator.insert_equation(cod, start, INTERPT_RMAX5, eM, n, aaM);
	for (int day = start, i = -1; force_until < day && i < m; day = utils::dec_day(day))
	{
		if (!empty_queue.empty() && empty_queue.front() == day)
		{
			empty_queue.pop_front();
			continue;
		}
		double ym = matrix::dot(n, aam, A + (n * i));
		dbinterpolator.insert_result(cod, day, INTERPT_RMIN5, ym, start);
		double m = (i >= 0) ? bbm[i] : ym;
		ILOG("Regression::calculate(%s, %d) -> (%d) min %g/%g (%g)", cod, day, i, ym, m, ym - m);
		double yM = matrix::dot(n, aaM, A + (n * i));
		dbinterpolator.insert_result(cod, day, INTERPT_RMAX5, yM, start);
		double M = (i >= 0) ? bbM[i] : yM;
		ILOG("Regression::calculate(%s, %d) -> (%d) max %g/%g (%g)", cod, day, i, yM, M, yM - M);

		if (!force_until)
			break;
		i++;
	}
cleanup:
	delete [] aam;
	delete [] aaM;
	delete [] AtA;
	delete [] L;
	delete [] U;
	delete [] Atbbm;
	delete [] AtbbM;
	delete [] bbm;
	delete [] bbM;
	delete [] uu;
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
