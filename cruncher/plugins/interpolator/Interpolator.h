#ifndef _INTERPOLATOR_H_
#define _INTERPOLATOR_H_

#include "DBfeeder.h"
#include "DBtrends.h"
#include "DBinterpolator.h"
#include "ICruncher.h"

class Interpolator : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBtrends dbtrends;
	DBinterpolator dbinterpolator;
	
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int state;
	int trends_updates;
	int force_until;

	void calculate(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);
	int get_state();

	Interpolator();
	~Interpolator();
};

#endif
