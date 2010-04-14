
#include <stdio.h>
#include "logger.h"
#include "DBfeeder.h"
#include "DBstatistics.h"

int main(int argc, char *argv[])
{
	INIT_DEFAULT_LOGGER;
	
	const char *conninfo = "";
	if (argc > 1) conninfo = argv[1];
	
	ILOG("testDB-> connecting with '%s' ...", conninfo);
	CcltorDB db;
	int r = db.connect(conninfo);
	ILOG("testDB-> db.init(%s) = %d - press enter to continue", conninfo, r);
	getchar();
	
	{
		DBfeeder dbfeeder(&db);
	
	ILOG("testDB-> sending feed 'test:556@testing.org', 116");
	r = dbfeeder.insert_feed("test:556@testing.org", 116);
	ILOG("testDB-> sending feed r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> sending price PR2 PRUEBA 2 1.12, 2.22, 3.32, 83600");
	r = dbfeeder.insert_price("PR2", "PRUEBA 2", 1.12, 2.22, 3.32, 83600);
	ILOG("testDB-> sending price r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get value codes");
	std::vector<std::string> codes;
	r = dbfeeder.get_value_codes(&codes);
	for (int i = 0; i < codes.size(); i++) DLOG("testDB-> code[%d] = %s", i, codes[i].c_str());
	ILOG("testDB-> get value codes r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get value of PR2");
	std::string name;
	r = dbfeeder.get_value_name("PR2", &name);
	DLOG("testDB-> name = %s", name.c_str());
	ILOG("testDB-> get value r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get_value_prices(PR2, 20100101, 50000, 20110205, 220002, ...)");
	std::vector<double> prices; std::vector<int> dates; std::vector<int> times;
	r = dbfeeder.get_value_prices("PR2", 20100101, 50000, 20110205, 220002, &prices, &dates, &times);
	for (int i = 0; i < prices.size(); i++) DLOG("testDB-> prices[%d] = %E", i, prices[i]);
	for (int i = 0; i < dates.size(); i++) DLOG("testDB-> dates[%d] = %08d", i, dates[i]);
	for (int i = 0; i < times.size(); i++) DLOG("testDB-> times[%d] = %06d", i, times[i]);
	ILOG("testDB-> get_value_prices r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get_value_volumes(PR2, 20100101, 50000, 20110205, 220002, ...)");
	std::vector<double> volumes; //std::vector<int> dates; std::vector<int> times;
	r = dbfeeder.get_value_volumes("PR2", 20100101, 50000, 20110205, 220002, &volumes, &dates, &times);
	for (int i = 0; i < volumes.size(); i++) DLOG("testDB-> volumes[%d] = %E", i, volumes[i]);
	for (int i = 0; i < dates.size(); i++) DLOG("testDB-> dates[%d] = %08d", i, dates[i]);
	for (int i = 0; i < times.size(); i++) DLOG("testDB-> times[%d] = %06d", i, times[i]);
	ILOG("testDB-> get_value_volumes r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get_value_capitals(PR2, 20100101, 50000, 20110205, 220002, ...)");
	std::vector<double> capitals; //std::vector<int> dates; std::vector<int> times;
	r = dbfeeder.get_value_capitals("PR2", 20100101, 50000, 20110205, 220002, &capitals, &dates, &times);
	for (int i = 0; i < capitals.size(); i++) DLOG("testDB-> capitals[%d] = %E", i, capitals[i]);
	for (int i = 0; i < dates.size(); i++) DLOG("testDB-> dates[%d] = %08d", i, dates[i]);
	for (int i = 0; i < times.size(); i++) DLOG("testDB-> times[%d] = %06d", i, times[i]);
	ILOG("testDB-> get_value_capitals r = %d - press enter to continue", r);
	getchar();
	}
	{
		DBstatistics DBstatistics(&db);
		
	ILOG("testDB-> insert_day PR2, 20100415, 1, 1, 1, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0, .5, .5, .5");
	r = DBstatistics.insert_day("PR2", 20100415, 1, 1, 1, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0, .5, .5, .5);
	ILOG("testDB-> insert_day r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> insert_month PR2, 20100415, 2, 2, 2, 1.20, 1.20, 1.20, 2.20, 2.20, 2.20, 3.20, 3.20, 3.20, .25, .25, .25");
	r = DBstatistics.insert_month("PR2", 20100415, 2, 2, 2, 1.20, 1.20, 1.20, 2.20, 2.20, 2.20, 3.20, 3.20, 3.20, .25, .25, .25);
	ILOG("testDB-> insert_month r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> insert_year PR2, 20100415, 3, 3, 3, 1.30, 1.30, 1.30, 2.30, 2.30, 2.30, 3.30, 3.30, 3.30, .35, .35, .35");
	r = DBstatistics.insert_year("PR2", 20100415, 3, 3, 3, 1.30, 1.30, 1.30, 2.30, 2.30, 2.30, 3.30, 3.30, 3.30, .35, .35, .35);
	ILOG("testDB-> insert_year r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get_day PR2, 20100415");
	int cnt_price, cnt_volume, cnt_capital;
	double min_price, min_volume, min_capital, mean_price, mean_volume, mean_capital;
	double max_price, max_volume, max_capital, std_price, std_volume, std_capital;
	r = DBstatistics.get_day("PR2", 20100415, &cnt_price, &cnt_volume, &cnt_capital,
			&min_price, &min_volume, &min_capital, &mean_price, &mean_volume, &mean_capital,
			&max_price, &max_volume, &max_capital, &std_price, &std_volume, &std_capital);
	DLOG("testDB-> get_day: cnt_price: %d, cnt_volume: %d, cnt_capital: %d", cnt_price, cnt_volume, cnt_capital);
	DLOG("testDB-> get_day: min_price: %E, min_volume: %E, min_capital: %E", min_price, min_volume, min_capital);
	DLOG("testDB-> get_day: mean_price: %E, mean_volume: %E, mean_capital: %E", mean_price, mean_volume, mean_capital);
	DLOG("testDB-> get_day: max_price: %E, max_volume: %E, max_capital: %E", max_price, max_volume, max_capital);
	DLOG("testDB-> get_day: std_price: %E, std_volume: %E, std_capital: %E", std_price, std_volume, std_capital);
	ILOG("testDB-> get_day r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get_month PR2, 20100415");
//	int cnt_price, cnt_volume, cnt_capital;
//	double min_price, min_volume, min_capital, mean_price, mean_volume, mean_capital;
//	double max_price, max_volume, max_capital, std_price, std_volume, std_capital;
	r = DBstatistics.get_month("PR2", 20100415, &cnt_price, &cnt_volume, &cnt_capital,
			&min_price, &min_volume, &min_capital, &mean_price, &mean_volume, &mean_capital,
			&max_price, &max_volume, &max_capital, &std_price, &std_volume, &std_capital);
	DLOG("testDB-> get_month: cnt_price: %d, cnt_volume: %d, cnt_capital: %d", cnt_price, cnt_volume, cnt_capital);
	DLOG("testDB-> get_month: min_price: %E, min_volume: %E, min_capital: %E", min_price, min_volume, min_capital);
	DLOG("testDB-> get_month: mean_price: %E, mean_volume: %E, mean_capital: %E", mean_price, mean_volume, mean_capital);
	DLOG("testDB-> get_month: max_price: %E, max_volume: %E, max_capital: %E", max_price, max_volume, max_capital);
	DLOG("testDB-> get_month: std_price: %E, std_volume: %E, std_capital: %E", std_price, std_volume, std_capital);
	ILOG("testDB-> get_month r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get_year PR2, 20100415");
//	int cnt_price, cnt_volume, cnt_capital;
//	double min_price, min_volume, min_capital, mean_price, mean_volume, mean_capital;
//	double max_price, max_volume, max_capital, std_price, std_volume, std_capital;
	r = DBstatistics.get_year("PR2", 20100415, &cnt_price, &cnt_volume, &cnt_capital,
			&min_price, &min_volume, &min_capital, &mean_price, &mean_volume, &mean_capital,
			&max_price, &max_volume, &max_capital, &std_price, &std_volume, &std_capital);
	DLOG("testDB-> get_year: cnt_price: %d, cnt_volume: %d, cnt_capital: %d", cnt_price, cnt_volume, cnt_capital);
	DLOG("testDB-> get_year: min_price: %E, min_volume: %E, min_capital: %E", min_price, min_volume, min_capital);
	DLOG("testDB-> get_year: mean_price: %E, mean_volume: %E, mean_capital: %E", mean_price, mean_volume, mean_capital);
	DLOG("testDB-> get_year: max_price: %E, max_volume: %E, max_capital: %E", max_price, max_volume, max_capital);
	DLOG("testDB-> get_year: std_price: %E, std_volume: %E, std_capital: %E", std_price, std_volume, std_capital);
	ILOG("testDB-> get_year r = %d - press enter to continue", r);
	getchar();
	}
	
	DLOG("testDB-> closing connection ...");
	db.release();
	DLOG("testDB-> we are done");
}
