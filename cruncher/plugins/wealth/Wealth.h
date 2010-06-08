#ifndef _WEALTH_H_
#define _WEALTH_H_

#include "DBfeeder.h"
#include "DBstatistics.h"
#include "DBtrends.h"
#include "DBwealth.h"
#include "ICruncher.h"

class Wealth : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	DBtrends dbtrends;
	DBwealth dbwealth;
	
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int state;
	int trends_updates;
	int force_until;
	
	void calculate(const char *cod, int start);
	void calculate_acum(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);
	int get_state();

	Wealth();
	~Wealth();
};

#endif
