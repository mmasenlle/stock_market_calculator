#ifndef _DBTRENDS_H_
#define _DBTRENDS_H_

#include <vector>
#include "CcltorDB.h"

enum
{
	TRENDS_P,
	TRENDS_R1,
	TRENDS_S1,
	TRENDS_R2,
	TRENDS_S2,
	TRENDS_R3,
	TRENDS_S3,
	TRENDS_R4,
	TRENDS_S4,
	TRENDS_MF,
	
	NR_TRENDS
};

enum
{
	TRENDS_ACUM_SMA,
	TRENDS_ACUM_MAD,
	TRENDS_ACUM_CCI,
	TRENDS_ACUM_ROC,
	TRENDS_ACUM_MFI,
	
	NR_TRENDS_ACUM
};

class DBtrends
{
	CcltorDB *mdb;

public:
	DBtrends(CcltorDB *db);

	int insert(const char *value, int yyyymmdd,
			double P, double R1, double S1, double R2, double S2,
			double R3, double S3, double R4, double S4, double MF);
	int insert_acum(const char *value, int yyyymmdd,
			double SMA, double MAD, double CCI, double ROC, double MFI);

	int get(const char *value, int trend, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days);
	int get_acum(const char *value, int trend, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days);
};

#endif
