#ifndef _DBWEALTH_H_
#define _DBWEALTH_H_

#include <vector>
#include "CcltorDB.h"

enum {
	WEALTH_ITEM_PRICE,
	WEALTH_ITEM_VOLUME,
	WEALTH_ITEM_CAPITAL,
	LAST_WEALTH_ITEM
};
enum {
	WEALTH_STC_COUNT,
	WEALTH_STC_OPEN,
	WEALTH_STC_CLOSE,
	WEALTH_STC_MIN,
	WEALTH_STC_MEAN,
	WEALTH_STC_MAX,
	WEALTH_STC_STD,
	LAST_WEALTH_STC
};

#define WEALTH_DEFAULT_MARKET "MADRID"

class DBwealth
{
	CcltorDB *mdb;

public:
	DBwealth(CcltorDB *db);

	int insert(int yyyymmdd,
			int cnt_price, int cnt_volume, int cnt_capital,
			double open_price, double open_volume, double open_capital,
			double close_price, double close_volume, double close_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital,
		    const char *market = WEALTH_DEFAULT_MARKET);

	int get(int item, int stc,
			int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days,
		    const char *market = WEALTH_DEFAULT_MARKET);
};

#endif
