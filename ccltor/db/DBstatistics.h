#ifndef DBSTATISTICS_H_
#define DBSTATISTICS_H_

#include "CcltorDB.h"

class DBstatistics
{
public:
	static int insert_day(CcltorDB *db,
			const char *value, int day,
			double cnt_price, double cnt_volume, double cnt_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital);
	static int insert_month(CcltorDB *db,
			const char *value, int month,
			double cnt_price, double cnt_volume, double cnt_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital);
	static int insert_year(CcltorDB *db,
			const char *value, int year,
			double cnt_price, double cnt_volume, double cnt_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital);

	static int get_day(CcltorDB *db,
			const char *value, int day,
			double &cnt_price, double &cnt_volume, double &cnt_capital,
			double &min_price, double &min_volume, double &min_capital,
			double &mean_price, double &mean_volume, double &mean_capital,
			double &max_price, double &max_volume, double &max_capital,
			double &std_price, double &std_volume, double &std_capital);
	static int get_month(CcltorDB *db,
			const char *value, int month,
			double &cnt_price, double &cnt_volume, double &cnt_capital,
			double &min_price, double &min_volume, double &min_capital,
			double &mean_price, double &mean_volume, double &mean_capital,
			double &max_price, double &max_volume, double &max_capital,
			double &std_price, double &std_volume, double &std_capital);
	static int get_year(CcltorDB *db,
			const char *value, int year,
			double &cnt_price, double &cnt_volume, double &cnt_capital,
			double &min_price, double &min_volume, double &min_capital,
			double &mean_price, double &mean_volume, double &mean_capital,
			double &max_price, double &max_volume, double &max_capital,
			double &std_price, double &std_volume, double &std_capital);
};

#endif /*DBSTATISTICS_H_*/
