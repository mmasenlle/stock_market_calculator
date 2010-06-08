#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "DBfeeder.h"
#include "DBstatistics.h"
#include "ICEvent.h"
#include "ICruncher.h"

class Statistics : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	ICEvent stcs_updated;

	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int state;
	int newfeeds;
	int force_until;
	
	int calculate(const std::vector<double> *data,
			double *open, double *close, double *min, double *mean, double *max, double *std);
	double meta_calculate(const std::vector<double> *data, int what);

	void calculate_days(const char *cod, int start);
	void calculate_months(const char *cod, int start);
	void calculate_years(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);
	int get_state();

	Statistics();
	~Statistics();
};

#endif
