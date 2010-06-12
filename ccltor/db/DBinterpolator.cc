
#include <stdlib.h>
#include "utils.h"
#include "DBinterpolator.h"

DBinterpolator::DBinterpolator(CcltorDB *db) : mdb(db) {}

int DBinterpolator::insert(const char *value, int yyyymmdd,
    double PP, double error, const double A[])
{
	int ret = 0;
	char buffer[1024];
	snprintf(buffer, sizeof(buffer),
			"INSERT INTO interpolator (value, date) VALUES ('%s', '%08d');",
			value, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer),
			"UPDATE interpolator SET PP = %.15G, error = %.15G, "
			"a0 = %.15G, a1 = %.15G, a2 = %.15G, a3 = %.15G, a4 = %.15G, "
			"a5 = %.15G, a6 = %.15G, a7 = %.15G, a8 = %.15G, a9 = %.15G, "
			"a10 = %.15G, a11 = %.15G, a12 = %.15G, a13 = %.15G, a14 = %.15G, "
			"a15 = %.15G, a16 = %.15G, a17 = %.15G, a18 = %.15G, a19 = %.15G "
			"WHERE value = '%s' AND date = '%08d';",
			PP, error, A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], A[8], A[9],
			A[10], A[11], A[12], A[13], A[14], A[15], A[16], A[17], A[18], A[19],
			value, yyyymmdd);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}


static const char *interpolator_names[NR_INTERPOLATOR] = {
		"PP", "error", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9",
		"a10", "a11", "a12", "a13", "a14", "a15", "a16", "a17", "a18", "a19"
};
int DBinterpolator::get(const char *value, int item, int yyyymmdd_start, int yyyymmdd_end,
		std::vector<double> *data, std::vector<int> *days)
{
	int ret = 0;
	if (item < NR_INTERPOLATOR)
	{	// 0, -1 means no limit
		if (yyyymmdd_start < 20000101) yyyymmdd_start = 20000101;
		if (yyyymmdd_end < 20000101) yyyymmdd_end = 20500101;
		char buffer[256];
		snprintf(buffer, sizeof(buffer),
				"SELECT %s, date FROM interpolator WHERE value = '%s' AND date >= '%08d' AND date <= '%08d' ORDER BY date;",
				interpolator_names[item], value, yyyymmdd_start, yyyymmdd_end);
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
