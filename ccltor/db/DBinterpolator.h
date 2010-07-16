#ifndef _DBINTERPOLATOR_H_
#define _DBINTERPOLATOR_H_

#include <vector>
#include "CcltorDB.h"

enum
{
	INTERPT_MIN5,
	INTERPT_MAX5,
	INTERPT_RMIN5,
	INTERPT_RMAX5,
};

class DBinterpolator
{
	CcltorDB *mdb;

public:
	DBinterpolator(CcltorDB *db);

	int insert_equation(const char *value, int yyyymmdd, int type,
			double error, int n, const double aa[]);
	int insert_result(const char *value, int yyyymmdd, int type,
			double result, int e_yyyymmdd = 0);

	int get_equations(const char *value, int type, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *errors, std::vector<int> *days, std::vector<int> *eq_ids);
	int get_coefficients(const char *value, int type, int yyyymmdd,
			int n, double *aa, int eq_id = 0);
	int get_results(const char *value, int type, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *results, std::vector<int> *days);
};

#endif
