
#include "utils.h"
#include "logger.h"
#include "DBCache.h"


DBCache::DBCache() : dbfeeder(&db), dbstatistics(&db), dbtrends(&db)
{
	int r = pthread_mutex_init(&mtx, NULL);
	if (r != 0)
	{
		ELOG("DBCache::DBCache() -> pthread_mutex_init(): %d", r);
	}
}

DBCache::~DBCache()
{
	int r = pthread_mutex_destroy(&mtx);
	if (r != 0)
	{
		ELOG("DBCache::~DBCache() -> pthread_mutex_destroy(): %d", r);
	}
}

void DBCache::init(const char *db_conninfo)
{
	if (db.connect(db_conninfo) != CONNECTION_OK)
	{
		ELOG("DBCache::init(%s)", db_conninfo);
	}
}

DBCacheEntry::DBCacheEntry(int t, time_t cur)
{
	expires = cur + t;
}

bool DBCacheEntry::valid(time_t cur)
{
	return ((expires > cur) && (expires < (cur + 50000)));
}

void DBCache::drain()
{
	pthread_mutex_lock(&mtx);
	std::map<std::string, DBCacheEntry *>::iterator j = cache.end();
	for (std::map<std::string, DBCacheEntry *>::iterator i = cache.begin();
			i != cache.end(); i++)
	{
		if (j != cache.end())
		{
			delete j->second;
			cache.erase(j);
			j = cache.end();
		}
		if (!i->second->valid())
		{
			j = i;
		}
	}
	if (j != cache.end())
	{
		delete j->second;
		cache.erase(j);
	}
	pthread_mutex_unlock(&mtx);
}

struct Entry_feeder__get_value_codes : public DBCacheEntry
{
	Entry_feeder__get_value_codes() : DBCacheEntry(5000) {};
	std::vector<std::string> codes;
};

int DBCache::feeder__get_value_codes(std::vector<std::string> *codes)
{
	int ret = 0;
	pthread_mutex_lock(&mtx);
	if (cache.find(__FUNCTION__) != cache.end())
	{
		if (cache[__FUNCTION__]->valid())
		{
			*codes = ((Entry_feeder__get_value_codes*)cache[__FUNCTION__])->codes;
			ret = 1;
		}
		else
		{
			delete cache[__FUNCTION__];
			cache.erase(__FUNCTION__);
		}
	}
	if (!ret)
	{
		if ((ret = dbfeeder.get_value_codes(codes)) > 0)
		{
			Entry_feeder__get_value_codes *e = new Entry_feeder__get_value_codes;
			e->codes = *codes;
			cache[__FUNCTION__] = e;
		}
	}
	pthread_mutex_unlock(&mtx);
	return ret;
}


struct Entry_scalar_data : public DBCacheEntry
{
	Entry_scalar_data(time_t cur) : DBCacheEntry(300, cur) {};
	double data;
};

int DBCache::statistics__get_day(const char *code, int item, int stc, int day, double *data)
{
	int ret = 0;
	char key[256];
	snprintf(key, sizeof(key), "%s(%s,%d,%d,%d)", __FUNCTION__, code, item, stc, day);
	pthread_mutex_lock(&mtx);
	if (cache.find(key) != cache.end())
	{
		if (cache[key]->valid())
		{
			*data = ((Entry_scalar_data*)cache[key])->data;
			ret = 1;
		}
		else
		{
			delete cache[key];
			cache.erase(key);
		}
	}
	if (!ret)
	{
		std::vector<double> d;
		dbstatistics.get_day(code, item, stc, day, day, &d, NULL);
		if (!d.empty())
		{
			*data = d.front();
			Entry_scalar_data *e = new Entry_scalar_data(time(NULL));
			e->data = *data;
			cache[key] = e;
			ret = 1;
		}
	}
	pthread_mutex_unlock(&mtx);
	return ret;
}

int DBCache::statistics__insert_day(const char *code, int day,
		double data[LAST_STATISTICS_ITEM][LAST_STATISTICS_STC])
{
	bool the_same = true;
	for (int j = 0; the_same && j < LAST_STATISTICS_ITEM; j++)
	{
		for (int k = 0; the_same && k < LAST_STATISTICS_STC; k++)
		{
			double d;
			the_same = (statistics__get_day(code, j, k, day, &d) > 0) && utils::equald(data[j][k], d);
		}
	}
	if (the_same)
	{
		return 0;
	}
	pthread_mutex_lock(&mtx);
	dbstatistics.insert_day(code, day, data[0][0], data[1][0], data[2][0],
			data[0][1], data[1][1], data[2][1], data[0][2], data[1][2], data[2][2],
			data[0][3], data[1][3], data[2][3], data[0][4], data[1][4], data[2][4],
			data[0][5], data[1][5], data[2][5], data[0][6], data[1][6], data[2][6]);
	time_t t = time(NULL);
	for (int j = 0; j < LAST_STATISTICS_ITEM; j++)
	{
		for (int k = 0; k < LAST_STATISTICS_STC; k++)
		{
			char key[256];
			snprintf(key, sizeof(key), "%s(%s,%d,%d,%d)", __FUNCTION__, code, j, k, day);
			if (cache.find(key) != cache.end())
			{
				delete cache[key];
				cache.erase(key);
			}
			Entry_scalar_data *e = new Entry_scalar_data(t);
			e->data = data[j][k];
			cache[key] = e;
		}
	}
	pthread_mutex_unlock(&mtx);
	return 1;
}

int DBCache::dbtrends__get(const char *code, int item, int day, double *data)
{
	int ret = 0;
	char key[256];
	snprintf(key, sizeof(key), "%s(%s,%d,%d)", __FUNCTION__, code, item, day);
	pthread_mutex_lock(&mtx);
	if (cache.find(key) != cache.end())
	{
		if (cache[key]->valid())
		{
			*data = ((Entry_scalar_data*)cache[key])->data;
			ret = 1;
		}
		else
		{
			delete cache[key];
			cache.erase(key);
		}
	}
	if (!ret)
	{
		std::vector<double> d;
		dbtrends.get(code, item, day, day, &d, NULL);
		if (!d.empty())
		{
			*data = d.front();
			Entry_scalar_data *e = new Entry_scalar_data(time(NULL));
			e->data = *data;
			cache[key] = e;
			ret = 1;
		}
	}
	pthread_mutex_unlock(&mtx);
	return ret;
}

int DBCache::dbtrends__insert(const char *code, int day, double data[NR_TRENDS])
{
	bool the_same = true;
	for (int j = 0; the_same && j < NR_TRENDS; j++)
	{
		double d;
		the_same = (dbtrends__get(code, j, day, &d) > 0) && utils::equald(data[j], d);
	}
	if (the_same)
	{
		return 0;
	}
	pthread_mutex_lock(&mtx);
	dbtrends.insert(code, day, data[0], data[1], data[2],
			data[3], data[4], data[5], data[6], data[7], data[8], data[9]);
	time_t t = time(NULL);
	for (int j = 0; j < NR_TRENDS; j++)
	{
		char key[256];
		snprintf(key, sizeof(key), "%s(%s,%d,%d)", __FUNCTION__, code, j, day);
		if (cache.find(key) != cache.end())
		{
			delete cache[key];
			cache.erase(key);
		}
		Entry_scalar_data *e = new Entry_scalar_data(t);
		e->data = data[j];
		cache[key] = e;
	}
	pthread_mutex_unlock(&mtx);
	return 1;
}
