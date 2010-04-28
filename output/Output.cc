
#include "logger.h"
#include "Table.h"
#include "Chart.h"
#include "Output.h"

Output::Output() : dbfeeder(&db), dbstatistics(&db)
{
	front = NULL;
}

Output::~Output()
{
	delete front;
}
	
void Output::get_data(std::vector<double> *data, std::vector<int> *dates, std::vector<int> *times)
{
	switch (config.type)
	{
	case OUTPTYPE_ALL: case	OUTPTYPE_OPEN: case OUTPTYPE_CLOSE:
		dbfeeder.get_value_data(config.value.c_str(), config.item - OUTPITEM_PRICE + FEEDER_DATAITEM_PRICE,
						config.day_start, config.time_start, config.day_end, config.time_end,
						data, dates, times);
		break;
	case OUTPTYPE_COUNT: case OUTPTYPE_MIN: case OUTPTYPE_MEAN: case OUTPTYPE_MAX: case OUTPTYPE_STD:
		dbstatistics.get_day(config.value.c_str(), config.item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				config.type - OUTPTYPE_COUNT + STATISTICS_STC_COUNT, config.day_start, config.day_end,
				data, dates);
		break;
	case OUTPTYPE_MCOUNT: case OUTPTYPE_MMIN: case OUTPTYPE_MMEAN: case OUTPTYPE_MMAX: case OUTPTYPE_MSTD:
		dbstatistics.get_month(config.value.c_str(), config.item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				config.type - OUTPTYPE_MCOUNT + STATISTICS_STC_COUNT, config.day_start, config.day_end,
				data, dates);
		break;
	case OUTPTYPE_YCOUNT: case OUTPTYPE_YMIN: case OUTPTYPE_YMEAN: case OUTPTYPE_YMAX: case OUTPTYPE_YSTD:
		dbstatistics.get_year(config.value.c_str(), config.item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				config.type - OUTPTYPE_YCOUNT + STATISTICS_STC_COUNT, config.day_start, config.day_end,
				data, dates);
		break;
	}
}

void Output::init()
{
	switch (config.output_mode)
	{
	case OUTPMODE_PLOT: case OUTPMODE_PNG:
		front = new Chart(&config);
		break;
	case OUTPMODE_HTML:
		front = new Table(&config);
		break;
	}
	if (db.connect(config.db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Output::init() -> db.connect(%s)", config.db_conninfo.c_str());
	}
}

void Output::output()
{
	if (front)
	{
		std::vector<double> data;
		std::vector<int> dates, times;
		get_data(&data, &dates, &times);
		front->output(&data, &dates, &times);
	}
}
