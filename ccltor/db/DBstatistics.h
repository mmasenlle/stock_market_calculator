#ifndef DBSTATISTICS_H_
#define DBSTATISTICS_H_

#include "CcltorDB.h"

class DBstatistics
{
	CcltorDB *mdb;

public:
	DBstatistics(CcltorDB *db);

	int insert_day(const char *value, int yyyymmdd,
			int cnt_price, int cnt_volume, int cnt_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital);
	int insert_month(const char *value, int yyyymmdd,
			int cnt_price, int cnt_volume, int cnt_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital);
	int insert_year(const char *value, int yyyymmdd,
			int cnt_price, int cnt_volume, int cnt_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital);

	int get_day(const char *value, int yyyymmdd,
			int *cnt_price, int *cnt_volume, int *cnt_capital,
			double *min_price, double *min_volume, double *min_capital,
			double *mean_price, double *mean_volume, double *mean_capital,
			double *max_price, double *max_volume, double *max_capital,
			double *std_price, double *std_volume, double *std_capital);
	int get_month(const char *value, int yyyymmdd,
			int *cnt_price, int *cnt_volume, int *cnt_capital,
			double *min_price, double *min_volume, double *min_capital,
			double *mean_price, double *mean_volume, double *mean_capital,
			double *max_price, double *max_volume, double *max_capital,
			double *std_price, double *std_volume, double *std_capital);
	int get_year(const char *value, int yyyymmdd,
			int *cnt_price, int *cnt_volume, int *cnt_capital,
			double *min_price, double *min_volume, double *min_capital,
			double *mean_price, double *mean_volume, double *mean_capital,
			double *max_price, double *max_volume, double *max_capital,
			double *std_price, double *std_volume, double *std_capital);
};

#endif /*DBSTATISTICS_H_*/
