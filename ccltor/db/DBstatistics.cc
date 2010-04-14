
#include <stdlib.h>
#include "DBstatistics.h"

DBstatistics::DBstatistics(CcltorDB *db) : mdb(db) {}

int DBstatistics::insert_day(const char *value, int yyyymmdd,
		int cnt_price, int cnt_volume, int cnt_capital,
		double min_price, double min_volume, double min_capital,
		double mean_price, double mean_volume, double mean_capital,
		double max_price, double max_volume, double max_capital,
		double std_price, double std_volume, double std_capital)
{
	int ret = 0;
	char buffer[1024];
	snprintf(buffer, sizeof(buffer),
			"INSERT INTO statistics_of_day (value, date) VALUES ('%s', '%08d');",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer),
			"UPDATE statistics_of_day SET cnt_price = %d, cnt_volume = %d, cnt_capital = %d, "
			"min_price = %E, min_volume = %E, min_capital = %E, "
			"mean_price = %E, mean_volume = %E, mean_capital = %E, "
			"max_price = %E, max_volume = %E, max_capital = %E, "
			"std_price = %E, std_volume = %E, std_capital = %E WHERE value = '%s' AND date = '%08d';",
			cnt_price, cnt_volume, cnt_capital,	min_price, min_volume, min_capital,
			mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital,
			std_price, std_volume, std_capital, value, yyyymmdd);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBstatistics::insert_month(const char *value, int yyyymmdd,
		int cnt_price, int cnt_volume, int cnt_capital,
		double min_price, double min_volume, double min_capital,
		double mean_price, double mean_volume, double mean_capital,
		double max_price, double max_volume, double max_capital,
		double std_price, double std_volume, double std_capital)
{
	int ret = 0;
	yyyymmdd = ((yyyymmdd / 100) * 100) + 1; //first day of the month
	char buffer[1024];
	snprintf(buffer, sizeof(buffer),
			"INSERT INTO statistics_of_month (value, date) VALUES ('%s', '%08d');",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer),
			"UPDATE statistics_of_month SET cnt_price = %d, cnt_volume = %d, cnt_capital = %d, "
			"min_price = %E, min_volume = %E, min_capital = %E, "
			"mean_price = %E, mean_volume = %E, mean_capital = %E, "
			"max_price = %E, max_volume = %E, max_capital = %E, "
			"std_price = %E, std_volume = %E, std_capital = %E WHERE value = '%s' AND date = '%08d';",
			cnt_price, cnt_volume, cnt_capital,	min_price, min_volume, min_capital,
			mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital,
			std_price, std_volume, std_capital, value, yyyymmdd);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBstatistics::insert_year(const char *value, int yyyymmdd,
		int cnt_price, int cnt_volume, int cnt_capital,
		double min_price, double min_volume, double min_capital,
		double mean_price, double mean_volume, double mean_capital,
		double max_price, double max_volume, double max_capital,
		double std_price, double std_volume, double std_capital)
{
	int ret = 0;
	yyyymmdd = ((yyyymmdd / 10000) * 10000) + 101; //first day of the year
	char buffer[1024];
	snprintf(buffer, sizeof(buffer),
			"INSERT INTO statistics_of_year (value, date) VALUES ('%s', '%08d');",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer),
			"UPDATE statistics_of_year SET cnt_price = %d, cnt_volume = %d, cnt_capital = %d, "
			"min_price = %E, min_volume = %E, min_capital = %E, "
			"mean_price = %E, mean_volume = %E, mean_capital = %E, "
			"max_price = %E, max_volume = %E, max_capital = %E, "
			"std_price = %E, std_volume = %E, std_capital = %E WHERE value = '%s' AND date = '%08d';",
			cnt_price, cnt_volume, cnt_capital,	min_price, min_volume, min_capital,
			mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital,
			std_price, std_volume, std_capital, value, yyyymmdd);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBstatistics::get_day(const char *value, int yyyymmdd,
		int *cnt_price, int *cnt_volume, int *cnt_capital,
		double *min_price, double *min_volume, double *min_capital,
		double *mean_price, double *mean_volume, double *mean_capital,
		double *max_price, double *max_volume, double *max_capital,
		double *std_price, double *std_volume, double *std_capital)
{
	int ret = 0;
	char buffer[512];
	snprintf(buffer, sizeof(buffer),
			"SELECT cnt_price, cnt_volume, cnt_capital, min_price, min_volume, min_capital, "
			"mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital, "
			"std_price, std_volume, std_capital FROM statistics_of_day "
			"WHERE value = '%s' AND date = '%08d';",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		char *s;
		ret = PQntuples(r);
		for (int i = 0; i < ret; i++)
		{
			if (cnt_price && (s = PQgetvalue(r, i, 0)))
				*cnt_price = strtol(s, NULL, 10);
			if (cnt_volume && (s = PQgetvalue(r, i, 1)))
				*cnt_volume = strtol(s, NULL, 10);
			if (cnt_capital && (s = PQgetvalue(r, i, 2)))
				*cnt_capital = strtol(s, NULL, 10);
			if (min_price && (s = PQgetvalue(r, i, 3)))
				*min_price = strtod(s, NULL);
			if (min_volume && (s = PQgetvalue(r, i, 4)))
				*min_volume = strtod(s, NULL);
			if (min_capital && (s = PQgetvalue(r, i, 5)))
				*min_capital = strtod(s, NULL);
			if (mean_price && (s = PQgetvalue(r, i, 6)))
				*mean_price = strtod(s, NULL);
			if (mean_volume && (s = PQgetvalue(r, i, 7)))
				*mean_volume = strtod(s, NULL);
			if (mean_capital && (s = PQgetvalue(r, i, 8)))
				*mean_capital = strtod(s, NULL);
			if (max_price && (s = PQgetvalue(r, i, 9)))
				*max_price = strtod(s, NULL);
			if (max_volume && (s = PQgetvalue(r, i, 10)))
				*max_volume = strtod(s, NULL);
			if (max_capital && (s = PQgetvalue(r, i, 11)))
				*max_capital = strtod(s, NULL);
			if (std_price && (s = PQgetvalue(r, i, 12)))
				*std_price = strtod(s, NULL);
			if (std_volume && (s = PQgetvalue(r, i, 13)))
				*std_volume = strtod(s, NULL);
			if (std_capital && (s = PQgetvalue(r, i, 14)))
				*std_capital = strtod(s, NULL);
		}
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBstatistics::get_month(const char *value, int yyyymmdd,
		int *cnt_price, int *cnt_volume, int *cnt_capital,
		double *min_price, double *min_volume, double *min_capital,
		double *mean_price, double *mean_volume, double *mean_capital,
		double *max_price, double *max_volume, double *max_capital,
		double *std_price, double *std_volume, double *std_capital)
{
	int ret = 0;
	yyyymmdd = ((yyyymmdd / 100) * 100) + 1; //first day of the month
	char buffer[512];
	snprintf(buffer, sizeof(buffer),
			"SELECT cnt_price, cnt_volume, cnt_capital, min_price, min_volume, min_capital, "
			"mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital, "
			"std_price, std_volume, std_capital FROM statistics_of_month "
			"WHERE value = '%s' AND date = '%08d';",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		char *s;
		ret = PQntuples(r);
		for (int i = 0; i < ret; i++)
		{
			if (cnt_price && (s = PQgetvalue(r, i, 0)))
				*cnt_price = strtol(s, NULL, 10);
			if (cnt_volume && (s = PQgetvalue(r, i, 1)))
				*cnt_volume = strtol(s, NULL, 10);
			if (cnt_capital && (s = PQgetvalue(r, i, 2)))
				*cnt_capital = strtol(s, NULL, 10);
			if (min_price && (s = PQgetvalue(r, i, 3)))
				*min_price = strtod(s, NULL);
			if (min_volume && (s = PQgetvalue(r, i, 4)))
				*min_volume = strtod(s, NULL);
			if (min_capital && (s = PQgetvalue(r, i, 5)))
				*min_capital = strtod(s, NULL);
			if (mean_price && (s = PQgetvalue(r, i, 6)))
				*mean_price = strtod(s, NULL);
			if (mean_volume && (s = PQgetvalue(r, i, 7)))
				*mean_volume = strtod(s, NULL);
			if (mean_capital && (s = PQgetvalue(r, i, 8)))
				*mean_capital = strtod(s, NULL);
			if (max_price && (s = PQgetvalue(r, i, 9)))
				*max_price = strtod(s, NULL);
			if (max_volume && (s = PQgetvalue(r, i, 10)))
				*max_volume = strtod(s, NULL);
			if (max_capital && (s = PQgetvalue(r, i, 11)))
				*max_capital = strtod(s, NULL);
			if (std_price && (s = PQgetvalue(r, i, 12)))
				*std_price = strtod(s, NULL);
			if (std_volume && (s = PQgetvalue(r, i, 13)))
				*std_volume = strtod(s, NULL);
			if (std_capital && (s = PQgetvalue(r, i, 14)))
				*std_capital = strtod(s, NULL);
		}
		PQclear(r);
		ret++;
	}
	return ret;
}

int DBstatistics::get_year(const char *value, int yyyymmdd,
		int *cnt_price, int *cnt_volume, int *cnt_capital,
		double *min_price, double *min_volume, double *min_capital,
		double *mean_price, double *mean_volume, double *mean_capital,
		double *max_price, double *max_volume, double *max_capital,
		double *std_price, double *std_volume, double *std_capital)
{
	int ret = 0;
	yyyymmdd = ((yyyymmdd / 10000) * 10000) + 101; //first day of the year
	char buffer[512];
	snprintf(buffer, sizeof(buffer),
			"SELECT cnt_price, cnt_volume, cnt_capital, min_price, min_volume, min_capital, "
			"mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital, "
			"std_price, std_volume, std_capital FROM statistics_of_year "
			"WHERE value = '%s' AND date = '%08d';",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		char *s;
		ret = PQntuples(r);
		for (int i = 0; i < ret; i++)
		{
			if (cnt_price && (s = PQgetvalue(r, i, 0)))
				*cnt_price = strtol(s, NULL, 10);
			if (cnt_volume && (s = PQgetvalue(r, i, 1)))
				*cnt_volume = strtol(s, NULL, 10);
			if (cnt_capital && (s = PQgetvalue(r, i, 2)))
				*cnt_capital = strtol(s, NULL, 10);
			if (min_price && (s = PQgetvalue(r, i, 3)))
				*min_price = strtod(s, NULL);
			if (min_volume && (s = PQgetvalue(r, i, 4)))
				*min_volume = strtod(s, NULL);
			if (min_capital && (s = PQgetvalue(r, i, 5)))
				*min_capital = strtod(s, NULL);
			if (mean_price && (s = PQgetvalue(r, i, 6)))
				*mean_price = strtod(s, NULL);
			if (mean_volume && (s = PQgetvalue(r, i, 7)))
				*mean_volume = strtod(s, NULL);
			if (mean_capital && (s = PQgetvalue(r, i, 8)))
				*mean_capital = strtod(s, NULL);
			if (max_price && (s = PQgetvalue(r, i, 9)))
				*max_price = strtod(s, NULL);
			if (max_volume && (s = PQgetvalue(r, i, 10)))
				*max_volume = strtod(s, NULL);
			if (max_capital && (s = PQgetvalue(r, i, 11)))
				*max_capital = strtod(s, NULL);
			if (std_price && (s = PQgetvalue(r, i, 12)))
				*std_price = strtod(s, NULL);
			if (std_volume && (s = PQgetvalue(r, i, 13)))
				*std_volume = strtod(s, NULL);
			if (std_capital && (s = PQgetvalue(r, i, 14)))
				*std_capital = strtod(s, NULL);
		}
		PQclear(r);
		ret++;
	}
	return ret;
}
