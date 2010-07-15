#ifndef _REGRESSION_H_
#define _REGRESSION_H_

#include "DBfeeder.h"
#include "DBstatistics.h"
#include "DBinterpolator.h"
#include "ICruncher.h"

class Regression : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	DBinterpolator dbinterpolator;
	
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int state;
	int stcs_updates;
	int force_until;

	void calculate(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);
	int get_state();

	Regression();
	~Regression();
};

#endif
