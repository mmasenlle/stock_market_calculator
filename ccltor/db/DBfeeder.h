#ifndef DBFEEDER_H_
#define DBFEEDER_H_

#include <vector>
#include <string>
#include "CcltorDB.h"

class DBfeeder
{
public:
	static int insert_feed(CcltorDB *db,
			const char *feeder, int count);
	static int insert_price(CcltorDB *db,
			const char *code, const char *name,
			double price, double volume, double capital,
			int hhmmss);

	static int get_value_codes(CcltorDB *db,
			std::vector<std::string> *codes);
	static int get_value_name(CcltorDB *db,
			const char *value, std::string *name);
	static int get_value_prices(CcltorDB *db,
			const char *value, int day_start, int sec_start, int day_end, int sec_end,
			std::vector<double> *prices);
	static int get_value_volumes(CcltorDB *db,
			const char *value, int day_start, int sec_start, int day_end, int sec_end,
			std::vector<double> *volumes);
	static int get_value_capitals(CcltorDB *db,
			const char *value, int day_start, int sec_start, int day_end, int sec_end,
			std::vector<double> *capitals);
};

#endif /*DBFEEDER_H_*/
