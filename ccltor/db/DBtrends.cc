
#include <stdlib.h>
#include "utils.h"
#include "DBtrends.h"

DBtrends::DBtrends(CcltorDB *db) : mdb(db) {}

int DBtrends::insert(const char *value, int yyyymmdd,
		double P, double R1, double S1, double R2, double S2,
		double R3, double S3, double R4, double S4)
{
	int ret = 0;
	char buffer[1024];
	snprintf(buffer, sizeof(buffer),
			"INSERT INTO trends (value, date) VALUES ('%s', '%08d');",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer),
			"UPDATE trends SET P = %.15G, R1 = %.15G, S1 = %.15G, "
			"R2 = %.15G, S2 = %.15G, R3 = %.15G, S3 = %.15G, R4 = %.15G, S4 = %.15G, "
			"WHERE value = '%s' AND date = '%08d';",
			P, R1, S1, R2, S2, R3, S3, R4, S4, value, yyyymmdd);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}


static const char *trends_trend_names[NR_TRENDS] = { "P", "R1", "S1", "R2", "S2", "R3", "S3", "R4", "S4" };
int DBtrends::get(const char *value, int trend, int yyyymmdd_start, int yyyymmdd_end,
		std::vector<double> *data, std::vector<int> *days)
{
	int ret = 0;
	if (trend < NR_TRENDS)
	{	// 0, -1 means no limit
		if (yyyymmdd_start < 20000101) yyyymmdd_start = 20000101;
		if (yyyymmdd_end < 20000101) yyyymmdd_end = 20500101;
		char buffer[256];
		snprintf(buffer, sizeof(buffer),
				"SELECT %s, date FROM trends WHERE value = '%s' AND date >= '%08d' AND date <= '%08d';",
				trends_trend_names[trend], value, yyyymmdd_start, yyyymmdd_end);
		PGresult *r = mdb->exec_sql(buffer);
		if (r)
		{
			ret = PQntuples(r);
			if (data) data->clear();
			if (days) days->clear();
			for (int i = 0; i < ret; i++)
			{
				if (data)
				{
					char *str = PQgetvalue(r, i, 0);
					if (str) data->push_back(strtod(str, NULL));
				}
				if (days)
				{
					char *str = PQgetvalue(r, i, 1);
					if (str) days->push_back(utils::strtot(str));
				}
			}
			PQclear(r);
		}
	}
	return ret;
}
