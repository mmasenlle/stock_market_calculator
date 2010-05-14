#ifndef _TRENDS_H_
#define _TRENDS_H_

#include "DBtrends.h"
#include "ICruncher.h"

class Trends : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBtrends dbtrends;
	
	int stcs_updates;
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int force_until;
	
	void calculate(const char *cod, int start);
	void calculate_acum(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);

	Trends();
	~Trends();
};

#endif
