
#include <stdlib.h>
#include "utils.h"
#include "DBfeeder.h"

DBfeeder::DBfeeder(CcltorDB *db) : mdb(db)
{
	insert_value_prepared = false;
	for (int i = 0; i < LAST_FEEDER_DATAITEM; i++)
		get_value_data_prepared[i] = false;
}

int DBfeeder::insert_value(const char *code, const char *name,
			double price, double volume, double capital, int hhmmss)
{
	int ret = 0;
	if (!insert_value_prepared)
	{
		PGresult *r = mdb->exec_sql(
			"PREPARE insert_value_data (text, time, double precision, double precision, double precision) AS "
			"INSERT INTO feeder_value_data (value, time, price, volume, capital) VALUES ($1, $2, $3, $4, $5);");
		if (r)
		{
			insert_value_prepared = true;
			PQclear(r);
			ret++;
		}
	}
	char buffer[256];
	if (values_cache.find(code) == values_cache.end())
	{
		snprintf(buffer, sizeof(buffer),
				"INSERT INTO feeder_value_ids (source_id, name) VALUES ('%s', '%s');",
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
	if (!insert_value_prepared)
	{
		snprintf(buffer, sizeof(buffer),
				"INSERT INTO feeder_value_data (value, time, price, volume, capital) "
				"VALUES ('%s', '%06d', %.15G, %.15G, %.15G);",
				code, hhmmss, price, volume, capital);
	}
	else
	{
		snprintf(buffer, sizeof(buffer),
				"EXECUTE insert_value_data ('%s', '%06d', %.15G, %.15G, %.15G);",
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
	PGresult *r = mdb->exec_sql("SELECT DISTINCT ccltor_id FROM feeder_value_ids WHERE ccltor_id NOTNULL;");
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
		"SELECT name FROM feeder_value_ids WHERE ccltor_id = '%s';", value);
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

static const char *value_item_names[] = { "price", "volume", "capital" };
int DBfeeder::get_value_data(const char *value, int item,
			int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
			std::vector<double> *data, std::vector<int> *dates, std::vector<int> *times)
{
	int ret = 0;
	if (item < ARRAY_SIZE(value_item_names))
	{	// 0, -1 means no limit
		if (yyyymmdd_start < 20000101) yyyymmdd_start = 20000101;
		if (yyyymmdd_end < 20000101) yyyymmdd_end = 20500101;
		if (hhmmss_start < 0) hhmmss_start = 0;
		if (hhmmss_end <= 0 || hhmmss_end > 235959) hhmmss_end = 235959;
		char buffer[512];
		if (!get_value_data_prepared[item])
		{
			snprintf(buffer, sizeof(buffer), "PREPARE get_value_%s (text, date, date, time, time) AS "
							"SELECT %s, date, time FROM feeder_value_data, feeder_value_ids WHERE value = source_id "
							"AND ccltor_id = $1 AND date >= $2 AND date <= $3 AND time >= $4 AND time <= $5 "
							"ORDER BY date, time;", value_item_names[item], value_item_names[item]);
			PGresult *r = mdb->exec_sql(buffer);
			if (r)
			{
				get_value_data_prepared[item] = true;
				PQclear(r);
				ret++;
			}
		}
		if (!get_value_data_prepared[item])
		{
			snprintf(buffer, sizeof(buffer), "SELECT %s, date, time FROM feeder_value_data, feeder_value_ids "
					"WHERE value = source_id AND ccltor_id = $1 AND date >= '%08d' AND date <= '%08d' AND "
					"time >= '%06d' AND time <= '%06d' ORDER BY date, time;",
					value_item_names[item], value, yyyymmdd_start, yyyymmdd_end, hhmmss_start, hhmmss_end);
		}
		else
		{
			snprintf(buffer, sizeof(buffer),
					"EXECUTE get_value_%s ('%s', '%08d', '%08d', '%06d', '%06d');",
					value_item_names[item], value, yyyymmdd_start, yyyymmdd_end, hhmmss_start, hhmmss_end);
		}
		PGresult *r = mdb->exec_sql(buffer);
		if (r)
		{
			ret = PQntuples(r);
			if (data) data->clear();
			if (dates) dates->clear();
			if (times) times->clear();
			for (int i = 0; i < ret; i++)
			{
				if (data)
				{
					char *str = PQgetvalue(r, i, 0);
					if (str) data->push_back(strtod(str, NULL));
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

	}
	return ret;
}
