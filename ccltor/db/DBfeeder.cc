
#include <stdlib.h>
#include "utils.h"
#include "DBfeeder.h"

DBfeeder::DBfeeder(CcltorDB *db) : mdb(db)
{
	insert_price_prepared = false;
	get_value_prices_prepared = false;
}

int DBfeeder::insert_feed(const char *feeder, int count)
{
	int ret = 0;
	char buffer[256];
	snprintf(buffer, sizeof(buffer),
		"INSERT INTO feeder_feeds (feeder, count) VALUES ('%s', %d);",
		feeder, count);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBfeeder::insert_price(const char *code, const char *name,
			double price, double volume, double capital, int hhmmss)
{
	int ret = 0;
	if (!insert_price_prepared)
	{
		PGresult *r = mdb->exec_sql(
			"PREPARE insert_price (text, time, double precision, double precision, double precision) AS "
			"INSERT INTO feeder_prices (value, time, price, volume, capital) VALUES($1, $2, $3, $4, $5);");
		if (r)
		{
			insert_price_prepared = true;
			PQclear(r);
			ret++;
		}
	}
	char buffer[256];
	if (values_cache.find(code) == values_cache.end())
	{
		snprintf(buffer, sizeof(buffer),
				"INSERT INTO feeder_values (code, name) VALUES ('%s', '%s');",
				code, name);
		PGresult *r = mdb->exec_sql(buffer);
		if (r)
		{
			PQclear(r);
			ret++;
		}
	}
	else if (values_cache[code] == hhmmss)
	{
		return ret;
	}
	if (!insert_price_prepared)
	{
		snprintf(buffer, sizeof(buffer),
				"INSERT INTO feeder_prices (value, time, price, volume, capital) "
				"VALUES ('%s', '%06d', %E, %E, %E);",
				code, hhmmss, price, volume, capital);
	}
	else
	{
		snprintf(buffer, sizeof(buffer),
				"EXECUTE insert_price('%s', '%06d', %E, %E, %E);",
				code, hhmmss, price, volume, capital);
	}
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		values_cache[code] = hhmmss;
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBfeeder::get_value_codes(std::vector<std::string> *codes)
{
	int ret = 0;
	PGresult *r = mdb->exec_sql("SELECT code FROM feeder_values;");
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

int DBfeeder::get_value_name(const char *value, std::string *name)
{
	int ret = 0;
	char buffer[256];
	snprintf(buffer, sizeof(buffer),
		"SELECT name FROM feeder_values WHERE feeder_values.code = '%s';", value);
	PGresult *r = mdb->exec_sql(buffer);
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

int DBfeeder::get_value_prices(const char *value,
		int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
		std::vector<double> *prices, std::vector<int> *dates, std::vector<int> *times)
{
	int ret = 0;
	char buffer[512];
	if (hhmmss_end == 0)
	{
		snprintf(buffer, sizeof(buffer), "SELECT price, date, time FROM feeder_prices, "
				"(SELECT date as mdate, min(time) as mtime FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d') GROUP BY date) AS m "
				"WHERE (value = '%s' AND date = mdate AND time = mtime) ORDER BY date;",
				value, yyyymmdd_start, yyyymmdd_end, value);
	}
	else if (hhmmss_start == 240000)
	{
		snprintf(buffer, sizeof(buffer), "SELECT price, date, time FROM feeder_prices, "
				"(SELECT date as mdate, max(time) as mtime FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d') GROUP BY date) AS m "
				"WHERE (value = '%s' AND date = mdate AND time = mtime) ORDER BY date;",
				value, yyyymmdd_start, yyyymmdd_end, value);
	}
	else
	{
		if (!get_value_prices_prepared)
		{
			PGresult *r = mdb->exec_sql(
					"PREPARE get_value_prices (text, date, date, time, time) AS "
					"SELECT price, date, time FROM feeder_prices WHERE "
					"(value = $1 AND date >= $2 AND date <= $3 AND time >= $4 AND time <= $5) "
					"ORDER BY date, time;");
			if (r)
			{
				get_value_prices_prepared = true;
				PQclear(r);
				ret++;
			}
		}
		if (!get_value_prices_prepared)
		{
			snprintf(buffer, sizeof(buffer), "SELECT price, date, time FROM feeder_prices WHERE "
					"(value = '%s' AND date >= '%08d' AND date <= '%08d' AND "
					"time >= '%06d' AND time <= '%06d') ORDER BY date, time;",
					value, yyyymmdd_start, yyyymmdd_end, hhmmss_start, hhmmss_end);
		}
		else
		{
			snprintf(buffer, sizeof(buffer),
					"EXECUTE get_value_prices('%s', '%08d', '%08d', '%06d', '%06d');",
					value, yyyymmdd_start, yyyymmdd_end, hhmmss_start, hhmmss_end);
		}
	}
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		ret = PQntuples(r);
		if (prices) prices->clear();
		if (dates) dates->clear();
		if (times) times->clear();
		for (int i = 0; i < ret; i++)
		{
			if (prices)
			{
				char *str = PQgetvalue(r, i, 0);
				if (str) prices->push_back(strtod(str, NULL));
			}
			if (dates)
			{
				char *str = PQgetvalue(r, i, 1);
				if (str) dates->push_back(utils::strtot(str));
			}
			if (times)
			{
				char *str = PQgetvalue(r, i, 2);
				if (str) times->push_back(utils::strtot(str));
			}
		}
		PQclear(r);
	}
	return ret;
}

int DBfeeder::get_value_volumes(const char *value,
		int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
		std::vector<double> *volumes, std::vector<int> *dates, std::vector<int> *times)
{
	int ret = 0;
	char buffer[512];
	if (hhmmss_end == 0)
	{
		snprintf(buffer, sizeof(buffer), "SELECT volume, date, time FROM feeder_prices, "
				"(SELECT date as mdate, min(time) as mtime FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d') GROUP BY date) AS m "
				"WHERE (value = '%s' AND date = mdate AND time = mtime) ORDER BY date;",
				value, yyyymmdd_start, yyyymmdd_end, value);
	}
	else if (hhmmss_start == 240000)
	{
		snprintf(buffer, sizeof(buffer), "SELECT volume, date, time FROM feeder_prices, "
				"(SELECT date as mdate, max(time) as mtime FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d') GROUP BY date) AS m "
				"WHERE (value = '%s' AND date = mdate AND time = mtime) ORDER BY date;",
				value, yyyymmdd_start, yyyymmdd_end, value);
	}
	else
	{
		snprintf(buffer, sizeof(buffer), "SELECT volume, date, time FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d' AND "
				"time >= '%06d' AND time <= '%06d') ORDER BY date, time;",
				value, yyyymmdd_start, yyyymmdd_end, hhmmss_start, hhmmss_end);
	}
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		ret = PQntuples(r);
		if (volumes) volumes->clear();
		if (dates) dates->clear();
		if (times) times->clear();
		for (int i = 0; i < ret; i++)
		{
			if (volumes)
			{
				char *str = PQgetvalue(r, i, 0);
				if (str) volumes->push_back(strtod(str, NULL));
			}
			if (dates)
			{
				char *str = PQgetvalue(r, i, 1);
				if (str) dates->push_back(utils::strtot(str));
			}
			if (times)
			{
				char *str = PQgetvalue(r, i, 2);
				if (str) times->push_back(utils::strtot(str));
			}
		}
		PQclear(r);
	}
	return ret;
}

int DBfeeder::get_value_capitals(const char *value,
		int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
		std::vector<double> *capitals, std::vector<int> *dates, std::vector<int> *times)
{
	int ret = 0;
	char buffer[512];
	if (hhmmss_end == 0)
	{
		snprintf(buffer, sizeof(buffer), "SELECT capital, date, time FROM feeder_prices, "
				"(SELECT date as mdate, min(time) as mtime FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d') GROUP BY date) AS m "
				"WHERE (value = '%s' AND date = mdate AND time = mtime) ORDER BY date;",
				value, yyyymmdd_start, yyyymmdd_end, value);
	}
	else if (hhmmss_start == 240000)
	{
		snprintf(buffer, sizeof(buffer), "SELECT capital, date, time FROM feeder_prices, "
				"(SELECT date as mdate, max(time) as mtime FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d') GROUP BY date) AS m "
				"WHERE (value = '%s' AND date = mdate AND time = mtime) ORDER BY date;",
				value, yyyymmdd_start, yyyymmdd_end, value);
	}
	else
	{
		snprintf(buffer, sizeof(buffer), "SELECT capital, date, time FROM feeder_prices WHERE "
				"(value = '%s' AND date >= '%08d' AND date <= '%08d' AND "
				"time >= '%06d' AND time <= '%06d') ORDER BY date, time;",
				value, yyyymmdd_start, yyyymmdd_end, hhmmss_start, hhmmss_end);
	}
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		ret = PQntuples(r);
		if (capitals) capitals->clear();
		if (dates) dates->clear();
		if (times) times->clear();
		for (int i = 0; i < ret; i++)
		{
			if (capitals)
			{
				char *str = PQgetvalue(r, i, 0);
				if (str) capitals->push_back(strtod(str, NULL));
			}
			if (dates)
			{
				char *str = PQgetvalue(r, i, 1);
				if (str) dates->push_back(utils::strtot(str));
			}
			if (times)
			{
				char *str = PQgetvalue(r, i, 2);
				if (str) times->push_back(utils::strtot(str));
			}
		}
		PQclear(r);
	}
	return ret;	
}
