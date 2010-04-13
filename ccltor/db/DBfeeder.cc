
#include "DBfeeder.h"

int DBfeeder::insert_feed(CcltorDB *db, const char *feeder, int count)
{
	int ret = 0;
	char buffer[256];
	snprintf(buffer, sizeof(buffer),
		"INSERT INTO feeder_feeds (feeder, count) VALUES ('%s', %d);",
		feeder, count);
	PGresult *r = db->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBfeeder::insert_price(CcltorDB *db, const char *code, const char *name,
			double price, double volume, double capital, int hhmmss)
{
	int ret = 0;
	char buffer[256];
	snprintf(buffer, sizeof(buffer),
		"INSERT INTO feeder_values (code, name) VALUES ('%s', '%s');",
		code, name);
	PGresult *r = db->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer),
		"INSERT INTO feeder_prices (value, time, price, volume, capital) "
		"VALUES ('%s', '%06d', %E, %E, %E);",
		code, hhmmss, price, volume, capital);
	if ((r = db->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBfeeder::get_value_codes(CcltorDB *db,
			std::vector<std::string> *codes)
{
	int ret = 0;
	PGresult *r = db->exec_sql("SELECT code FROM feeder_values;");
	if (r)
	{
		ret = PQntuples(r);
		codes->clear();
		for (int i = 0; i < ret; i++)
		{
			char *str = PQgetvalue(r, i, 0);
			if (str) codes->push_back(str);
		}
		PQclear(r);
	}
	return ret;
}

int DBfeeder::get_value_name(CcltorDB *db,
			const char *value, std::string *name)
{
	int ret = 0;
	char buffer[256];
	snprintf(buffer, sizeof(buffer),
		"SELECT name FROM feeder_values WHERE feeder_values.code = '%s';", value);
	PGresult *r = db->exec_sql(buffer);
	if (r)
	{
		if ((ret = PQntuples(r)))
		{
			name->clear();
			char *str = PQgetvalue(r, 0, 0);
			if (str) *name = str;
		}
		PQclear(r);
	}
	return ret;
}

int DBfeeder::get_value_prices(CcltorDB *db,
			const char *value, int day_start, int sec_start, int day_end, int sec_end,
			std::vector<double> *prices)
{
	int ret = 0;
	
	return ret;
}

int DBfeeder::get_value_volumes(CcltorDB *db,
			const char *value, int day_start, int sec_start, int day_end, int sec_end,
			std::vector<double> *volumes)
{
	int ret = 0;
	
	return ret;
}

int DBfeeder::get_value_capitals(CcltorDB *db,
			const char *value, int day_start, int sec_start, int day_end, int sec_end,
			std::vector<double> *capitals)
{
	int ret = 0;
	
	return ret;
}

