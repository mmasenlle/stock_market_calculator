#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "DBfeeder.h"
#include "DBstatistics.h"
#include "ICruncher.h"

class Statistics : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	
	int newfeeds;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	
	void calculate(const std::vector<double> *data,
			double *min, double *mean, double *max, double *std);
	void calculate_days(const char *cod, int start);
	void calculate_months(const char *cod, int start);
	void calculate_years(const char *cod, int start);

public:
	int init(ICruncherManager *manager, Logger *logger);
	int run();
	int msg(ICMsg *msg);

	Statistics();
	~Statistics();
};

#endif