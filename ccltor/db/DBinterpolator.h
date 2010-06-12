#ifndef _DBINTERPOLATOR_H_
#define _DBINTERPOLATOR_H_

#include <vector>
#include "CcltorDB.h"

enum
{
	INTERPOLATOR_PP,
	INTERPOLATOR_error,
	INTERPOLATOR_a0,
	INTERPOLATOR_a1,
	INTERPOLATOR_a2,
	INTERPOLATOR_a3,
	INTERPOLATOR_a4,
	INTERPOLATOR_a5,
	INTERPOLATOR_a6,
	INTERPOLATOR_a7,
	INTERPOLATOR_a8,
	INTERPOLATOR_a9,
	INTERPOLATOR_a10,
	INTERPOLATOR_a11,
	INTERPOLATOR_a12,
	INTERPOLATOR_a13,
	INTERPOLATOR_a14,
	INTERPOLATOR_a15,
	INTERPOLATOR_a16,
	INTERPOLATOR_a17,
	INTERPOLATOR_a18,
	INTERPOLATOR_a19,
	
	NR_INTERPOLATOR
};

class DBinterpolator
{
	CcltorDB *mdb;

public:
	DBinterpolator(CcltorDB *db);

	int insert(const char *value, int yyyymmdd, double PP, double error, const double A[]);

	int get(const char *value, int item, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days);
};

#endif
