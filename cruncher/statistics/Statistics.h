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

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);

	Statistics();
	~Statistics();
};

#endif
