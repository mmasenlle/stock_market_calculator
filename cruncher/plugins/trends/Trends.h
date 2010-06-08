#ifndef _TRENDS_H_
#define _TRENDS_H_

#include "DBfeeder.h"
#include "DBstatistics.h"
#include "DBtrends.h"
#include "ICruncher.h"

class Trends : public ICruncher
{
	ICruncherManager *manager;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	DBtrends dbtrends;
	ICEvent trends_updated;
	
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int state;
	int stcs_updates;
	int force_until;
	
	void calculate(const char *cod, int start);
	void calculate_acum(const char *cod, int start);

public:
	int init(ICruncherManager *manager);
	int run();
	int msg(ICMsg *msg);
	int get_state();

	Trends();
	~Trends();
};

#endif
