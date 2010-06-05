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
enum
{
	WEALTH_TRENDS_P,
	WEALTH_TRENDS_R1,
	WEALTH_TRENDS_S1,
	WEALTH_TRENDS_R2,
	WEALTH_TRENDS_S2,
	WEALTH_TRENDS_R3,
	WEALTH_TRENDS_S3,
	WEALTH_TRENDS_R4,
	WEALTH_TRENDS_S4,
	WEALTH_TRENDS_MF,
	LAST_WEALTH_TRENDS
};
enum
{
	WEALTH_TRENDS_ACUM_SMA,
	WEALTH_TRENDS_ACUM_MAD,
	WEALTH_TRENDS_ACUM_CCI,
	WEALTH_TRENDS_ACUM_ROC,
	WEALTH_TRENDS_ACUM_AD,
	WEALTH_TRENDS_ACUM_MFI,
	WEALTH_TRENDS_ACUM_OBV,
	LAST_WEALTH_TRENDS_ACUM
};

#define WEALTH_DEFAULT_MARKET "MADRID"

class DBwealth
{
	CcltorDB *mdb;

public:
	DBwealth(CcltorDB *db);

	int insert_sday(int yyyymmdd,
			int cnt_price, int cnt_volume, int cnt_capital,
			double open_price, double open_volume, double open_capital,
			double close_price, double close_volume, double close_capital,
			double min_price, double min_volume, double min_capital,
			double mean_price, double mean_volume, double mean_capital,
			double max_price, double max_volume, double max_capital,
			double std_price, double std_volume, double std_capital,
		    const char *market = WEALTH_DEFAULT_MARKET);
	int insert_trends(int yyyymmdd,
			double P, double R1, double S1, double R2, double S2,
			double R3, double S3, double R4, double S4, double MF,
		    const char *market = WEALTH_DEFAULT_MARKET);
	int insert_trends_acum(int yyyymmdd,
			double SMA, double MAD, double CCI, double ROC, double AD, double MFI, double OBV,
		    const char *market = WEALTH_DEFAULT_MARKET);

	int get_sday(int item, int stc,
			int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days,
		    const char *market = WEALTH_DEFAULT_MARKET);
	int get_trends(int trend, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days,
		    const char *market = WEALTH_DEFAULT_MARKET);
	int get_trends_acum(int trend, int yyyymmdd_start, int yyyymmdd_end,
			std::vector<double> *data, std::vector<int> *days,
		    const char *market = WEALTH_DEFAULT_MARKET);
};

#endif
