
#include <stdlib.h>
#include "utils.h"
#include "DBinterpolator.h"

DBinterpolator::DBinterpolator(CcltorDB *db) : mdb(db) {}

int DBinterpolator::insert_equation(const char *value, int yyyymmdd, int type,
		double error, int n, const double aa[])
{
	int ret = 0;
	char buffer[256];
	snprintf(buffer, sizeof(buffer),
				"INSERT INTO interpolator_equation (value, date, type) VALUES ('%s', '%08d', %d);",
				value, yyyymmdd, type);
	PGresult *r = mdb->exec_sql(buffer);
	if (r) PQclear(r);
#ifdef DBINTERPOLATOR_ST_EQUATION_DATA
	snprintf(buffer, sizeof(buffer),
			"SELECT id FROM interpolator_equation WHERE value = '%s' AND date = '%08d' AND type = %d;",
			value, yyyymmdd, type);
	int id = 0;
	if ((r = mdb->exec_sql(buffer)))
	{
		ret = PQntuples(r);
		char *str = PQgetvalue(r, 0, 0);
		if (str) id = atoi(str);
		PQclear(r);
	}
	if (!id) return -1;
	snprintf(buffer, sizeof(buffer),
			"UPDATE interpolator_equation SET error = %.15G WHERE id = %d;", error, id);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	for (int i = 0; i < n; i++)
	{
		snprintf(buffer, sizeof(buffer),
				"INSERT INTO interpolator_coefficients (id, num) VALUES (%d, %d); ",
				id, i);
		if ((r = mdb->exec_sql(buffer)))
		{
			PQclear(r);
			ret++;
		}
		snprintf(buffer, sizeof(buffer),
				"UPDATE interpolator_coefficients SET val = %.15G WHERE id = %d AND num = %d;",
				aa[i], id, i);
		if ((r = mdb->exec_sql(buffer)))
		{
			PQclear(r);
			ret++;
		}
	}
#endif
	return ret;
}

int DBinterpolator::insert_result(const char *value, int yyyymmdd, int type,
		double result, int e_yyyymmdd)
{
	int ret = 0;
	char buffer[512];
	if (e_yyyymmdd)
	{
		snprintf(buffer, sizeof(buffer),
				"SELECT id FROM interpolator_equation WHERE value = '%s' AND date = '%08d' AND type = %d;",
				value, e_yyyymmdd, type);
	}
	else
	{
		snprintf(buffer, sizeof(buffer),
				"SELECT id FROM interpolator_equation WHERE value = '%s' AND type = %d "
				"ORDER BY date DESC;",
				value, type);
	}
	PGresult *r = mdb->exec_sql(buffer);
	int id = 0;
	if (r)
	{
		ret = PQntuples(r);
		char *str = PQgetvalue(r, 0, 0);
		if (str) id = atoi(str);
		PQclear(r);
	}
	if (id > 0)
	{
		snprintf(buffer, sizeof(buffer),
				"INSERT INTO interpolator_results (value, date, equation) VALUES ('%s', '%08d', %d); ",
				value, yyyymmdd, id);
		if ((r = mdb->exec_sql(buffer)))
		{
			PQclear(r);
			ret++;
		}
		snprintf(buffer, sizeof(buffer),
				"UPDATE interpolator_results SET result = %.15G "
			    "WHERE value = '%s' AND date = '%08d' AND equation = %d;",
				result, value, yyyymmdd, id);
		if ((r = mdb->exec_sql(buffer)))
		{
			PQclear(r);
			ret++;
		}
	}
	return ret;
}


int DBinterpolator::get_equations(const char *value, int type, int yyyymmdd_start, int yyyymmdd_end,
		std::vector<double> *errors, std::vector<int> *days, std::vector<int> *eq_ids)
{
	int ret = 0;
	if (yyyymmdd_start < 20000101) yyyymmdd_start = 20000101;
	if (yyyymmdd_end < 20000101) yyyymmdd_end = 20500101;
	char buffer[512];
	snprintf(buffer, sizeof(buffer),
			"SELECT error, date, id FROM interpolator_equation "
			"WHERE type = %d AND value = '%s' AND date >= '%08d' AND date <= '%08d' "
			"ORDER BY date, id;",
			type, value, yyyymmdd_start, yyyymmdd_end);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		ret = PQntuples(r);
		if (errors) errors->clear();
		if (days) days->clear();
		if (eq_ids) eq_ids->clear();
		for (int i = 0; i < ret; i++)
		{
			if (errors)
			{
				char *str = PQgetvalue(r, i, 0);
				if (str) errors->push_back(strtod(str, NULL));
			}
			if (days)
			{
				char *str = PQgetvalue(r, i, 1);
				if (str) days->push_back(utils::strtot(str));
			}
			if (eq_ids)
			{
				char *str = PQgetvalue(r, i, 0);
				if (str) eq_ids->push_back(strtol(str, NULL, 10));
			}
		}
		PQclear(r);
	}
	return ret;
}

int DBinterpolator::get_coefficients(const char *value, int type, int yyyymmdd,
		int n, double *aa, int eq_id)
{
	int ret = 0;
	char buffer[512];
	if (eq_id)
	{
		snprintf(buffer, sizeof(buffer),
				"SELECT val FROM interpolator_coefficients WHERE id = %d ORDER BY num;",
				eq_id);
	}
	else
	{
		snprintf(buffer, sizeof(buffer),
				"SELECT val FROM interpolator_coefficients, interpolator_equation "
				"WHERE interpolator_coefficients.id = interpolator_equation.id "
				"AND type = %d AND value = '%s' AND date = '%08d' ORDER BY num;",
				type, value, yyyymmdd);
	}
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		ret = PQntuples(r);
		for (int i = 0; i < ret && i < n; i++)
		{
			char *str = PQgetvalue(r, i, 0);
			if (str) aa[i] = strtod(str, NULL);
		}
		PQclear(r);
	}
	return ret;
}

int DBinterpolator::get_results(const char *value, int type, int yyyymmdd_start, int yyyymmdd_end,
		std::vector<double> *results, std::vector<int> *days)
{
	int ret = 0;
	if (yyyymmdd_start < 20000101) yyyymmdd_start = 20000101;
	if (yyyymmdd_end < 20000101) yyyymmdd_end = 20500101;
	char buffer[512];
	snprintf(buffer, sizeof(buffer),
			"SELECT result, interpolator_equation.date FROM interpolator_results, interpolator_equation "
			"WHERE type = %d AND interpolator_results.value = '%s' AND id = equation "
			"AND interpolator_equation.date >= '%08d' AND interpolator_equation.date <= '%08d' "
			"ORDER BY interpolator_equation.date, equation;",
			type, value, yyyymmdd_start, yyyymmdd_end);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		ret = PQntuples(r);
		if (results) results->clear();
		if (days) days->clear();
		for (int i = 0; i < ret; i++)
		{
			if (results)
			{
				char *str = PQgetvalue(r, i, 0);
				if (str) results->push_back(strtod(str, NULL));
			}
			if (days)
			{
				char *str = PQgetvalue(r, i, 1);
				if (str) days->push_back(utils::strtot(str));
			}
		}
		PQclear(r);
	}
	return ret;
}
