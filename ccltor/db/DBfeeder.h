#ifndef DBFEEDER_H_
#define DBFEEDER_H_

#include <map>
#include <vector>
#include <string>
#include "CcltorDB.h"

class DBfeeder
{
	CcltorDB *mdb;
	bool insert_price_prepared;
	bool get_value_prices_prepared;

	std::map<std::string, int> values_cache;

public:
	DBfeeder(CcltorDB *db);

	int insert_feed(const char *feeder, int count);
	int insert_price(const char *code, const char *name,
			double price, double volume, double capital,
			int hhmmss);

	int get_value_codes(std::vector<std::string> *codes);
	int get_value_name(const char *value, std::string *name);
/*
 * time_start <= (got values will be) <= time_end
 * hhmmss_end == 0 -> first values of the days
 * hhmmss_start == 240000 -> last values of the days
 */
	int get_value_prices(const char *value,
			int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
			std::vector<double> *prices, std::vector<int> *dates, std::vector<int> *times);
	int get_value_volumes(const char *value,
			int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
			std::vector<double> *volumes, std::vector<int> *dates, std::vector<int> *times);
	int get_value_capitals(const char *value,
			int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
			std::vector<double> *capitals, std::vector<int> *dates, std::vector<int> *times);
};

#endif /*DBFEEDER_H_*/
