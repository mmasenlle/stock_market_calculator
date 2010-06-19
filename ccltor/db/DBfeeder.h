#ifndef _DBFEEDER_H_
#define _DBFEEDER_H_

#include <map>
#include <vector>
#include <string>
#include "CcltorDB.h"

enum { FEEDER_DATAITEM_PRICE, FEEDER_DATAITEM_VOLUME, FEEDER_DATAITEM_CAPITAL, LAST_FEEDER_DATAITEM };

class DBfeeder
{
	CcltorDB *mdb;
	bool insert_value_prepared;
	bool get_value_data_prepared[LAST_FEEDER_DATAITEM];

	std::map<std::string, std::string> codes;
	std::map<std::string, int> values_cache;

public:
	DBfeeder(CcltorDB *db);

//	int insert_feed(const char *feeder, int count);
	int insert_value(const char *source_id, const char *name,
			double price, double volume, double capital,
			int hhmmss);

	int get_value_codes(std::vector<std::string> *codes);
	int get_value_name(const char *value, std::string *name);
	int get_value_data(const char *value, int item,
			int yyyymmdd_start, int hhmmss_start, int yyyymmdd_end, int hhmmss_end,
			std::vector<double> *data, std::vector<int> *dates, std::vector<int> *times);
};

#endif
