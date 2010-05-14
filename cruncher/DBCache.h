#ifndef _DBCACHE_H_
#define _DBCACHE_H_

#include <string>
#include <map>
#include <pthread.h>
#include "CcltorDB.h"
#include "DBfeeder.h"
#include "DBstatistics.h"
#include "DBtrends.h"

class DBCacheEntry
{
	time_t expires;
public:
	DBCacheEntry(int t = 300, time_t cur = time(NULL));
	bool valid(time_t cur = time(NULL));
	virtual ~DBCacheEntry() {};
};

class DBCache
{
	pthread_mutex_t mtx;

	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	DBtrends dbtrends;
	
	std::map<std::string, DBCacheEntry *> cache;

public:
	DBCache();
	~DBCache();
	void init(const char *db_conninfo);
	void drain();
	
	int feeder__get_value_codes(std::vector<std::string> *codes);
	int statistics__get_day(const char *code, int item, int stc, int day, double *data);
	int statistics__insert_day(const char *code, int day, double data[LAST_STATISTICS_ITEM][LAST_STATISTICS_STC]);
	int dbtrends__get(const char *code, int item, int day, double *data);
	int dbtrends__insert(const char *code, int day, double data[NR_TRENDS]);
};

#endif
