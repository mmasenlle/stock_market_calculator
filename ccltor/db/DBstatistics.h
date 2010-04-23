#ifndef _DBSTATISTICS_H_
#define _DBSTATISTICS_H_

#include <vector>
#include "CcltorDB.h"

enum {
	STATISTICS_ITEM_PRICE,
	STATISTICS_ITEM_VOLUME,
	STATISTICS_ITEM_CAPITAL,
	LAST_STATISTICS_ITEM
};
enum {
	STATISTICS_STC_COUNT,
	STATISTICS_STC_MIN,
	STATISTICS_STC_MEAN,
	STATISTICS_STC_MAX,
	STATISTICS_STC_STD,
	LAST_STATISTICS_STC
};

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

	int get_day(const char *value, int item, int stc, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days);
	int get_month(const char *value, int item, int stc, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *months);
	int get_year(const char *value, int item, int stc, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *years);
};

#endif
