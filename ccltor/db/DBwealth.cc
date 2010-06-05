
#include <stdlib.h>
#include "utils.h"
#include "DBwealth.h"

DBwealth::DBwealth(CcltorDB *db) : mdb(db) {}

static const char *wealth_update_sql_fmt =
	"UPDATE wealth SET cnt_price = %d, cnt_volume = %d, cnt_capital = %d, "
	"open_price = %.15G, open_volume = %.15G, open_capital = %.15G, "
	"close_price = %.15G, close_volume = %.15G, close_capital = %.15G, "
	"min_price = %.15G, min_volume = %.15G, min_capital = %.15G, "
	"mean_price = %.15G, mean_volume = %.15G, mean_capital = %.15G, "
	"max_price = %.15G, max_volume = %.15G, max_capital = %.15G, "
	"std_price = %.15G, std_volume = %.15G, std_capital = %.15G WHERE market = '%s' AND date = '%08d';";
int DBwealth::insert(int yyyymmdd,
		int cnt_price, int cnt_volume, int cnt_capital,
		double open_price, double open_volume, double open_capital,
		double close_price, double close_volume, double close_capital,
		double min_price, double min_volume, double min_capital,
		double mean_price, double mean_volume, double mean_capital,
		double max_price, double max_volume, double max_capital,
		double std_price, double std_volume, double std_capital,
	    const char *market)
{
	int ret = 0;
	char buffer[1024];
	snprintf(buffer, sizeof(buffer),
			"INSERT INTO wealth (market, date) VALUES ('%s', '%08d');",
			market, yyyymmdd);
	PGresult *r = mdb->exec_sql(buffer);
	if (r)
	{
		PQclear(r);
		ret++;
	}
	snprintf(buffer, sizeof(buffer), wealth_update_sql_fmt,
			cnt_price, cnt_volume, cnt_capital,	open_price, open_volume, open_capital,
			close_price, close_volume, close_capital, min_price, min_volume, min_capital,
			mean_price, mean_volume, mean_capital, max_price, max_volume, max_capital,
			std_price, std_volume, std_capital, market, yyyymmdd);
	if ((r = mdb->exec_sql(buffer)))
	{
		PQclear(r);
		ret++;
	}
	return ret;
}


static const char *wealth_item_names[LAST_WEALTH_STC][LAST_WEALTH_ITEM] = {
		{ "cnt_price", "cnt_volume", "cnt_capital" },
		{ "open_price", "open_volume", "open_capital" },
		{ "close_price", "close_volume", "close_capital" },
		{ "min_price", "min_volume", "min_capital" },
		{ "mean_price", "mean_volume", "mean_capital" },
		{ "max_price", "max_volume", "max_capital" },
		{ "std_price", "std_volume", "std_capital" },
};
static const char *wealth_get_sql_fmt =
	"SELECT %s, date FROM wealth WHERE market = '%s' AND date >= '%08d' AND date <= '%08d' ORDER BY date;";
int DBwealth::get(int item, int stc, int yyyymmdd_start, int yyyymmdd_end,
		std::vector<double> *data, std::vector<int> *days,
	    const char *market)
{
	int ret = 0;
	if (item < LAST_WEALTH_ITEM && stc < LAST_WEALTH_STC)
	{	// 0, -1 means no limit
		if (yyyymmdd_start < 20000101) yyyymmdd_start = 20000101;
		if (yyyymmdd_end < 20000101) yyyymmdd_end = 20500101;
		char buffer[256];
		snprintf(buffer, sizeof(buffer), wealth_get_sql_fmt, wealth_item_names[stc][item],
				market, yyyymmdd_start, yyyymmdd_end);
		PGresult *r = mdb->exec_sql(buffer);
		if (r)
		{
			ret = PQntuples(r);
			if (data) data->clear();
			if (days) days->clear();
			for (int i = 0; i < ret; i++)
			{
				if (data)
				{
					char *str = PQgetvalue(r, i, 0);
					if (str) data->push_back(strtod(str, NULL));
				}
				if (days)
				{
					char *str = PQgetvalue(r, i, 1);
					if (str) days->push_back(utils::strtot(str));
				}
			}
			PQclear(r);
		}
	}
	return ret;
}
