#ifndef _WEALTH_H_
#define _WEALTH_H_

#include "DBtrends.h"
#include "DBwealth.h"
#include "ICruncher.h"

class Wealth : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBtrends dbtrends;
	DBwealth dbwealth;
	
	int trends_updates;
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int force_until;
	
	void calculate(const char *cod, int start);
	void calculate_acum(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);

	Wealth();
	~Wealth();
};

#endif
