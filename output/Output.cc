#include <math.h>
#include "logger.h"
#include "Table.h"
#include "Chart.h"
#include "Output.h"

Output::Output()
 : dbfeeder(&db), dbstatistics(&db), dbtrends(&db), dbwealth(&db), dbinterpolator(&db)
{
	front = NULL;
}

Output::~Output()
{
	delete front;
}
	
void Output::get_data(const OutpDesc *desc, std::vector<double> *data, std::vector<int> times[2])
{
	switch (desc->type)
	{
	case OUTPTYPE_ALL:
		dbfeeder.get_value_data(desc->value.c_str(), desc->item - OUTPITEM_PRICE + FEEDER_DATAITEM_PRICE,
				desc->day_start, desc->time_start, desc->day_end, desc->time_end,
				data, &times[0], &times[1]);
		break;
	case OUTPTYPE_COUNT ... OUTPTYPE_STD:
		dbstatistics.get_day(desc->value.c_str(), desc->item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				desc->type - OUTPTYPE_COUNT + STATISTICS_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_MCOUNT ... OUTPTYPE_MSTD:
		dbstatistics.get_month(desc->value.c_str(), desc->item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				desc->type - OUTPTYPE_MCOUNT + STATISTICS_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_YCOUNT ... OUTPTYPE_YSTD:
		dbstatistics.get_year(desc->value.c_str(), desc->item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				desc->type - OUTPTYPE_YCOUNT + STATISTICS_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_P ... OUTPTYPE_MF:
		dbtrends.get(desc->value.c_str(), desc->type - OUTPTYPE_P + TRENDS_P,
				desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_SMA ... OUTPTYPE_OBV:
		dbtrends.get_acum(desc->value.c_str(), desc->type - OUTPTYPE_SMA + TRENDS_ACUM_SMA,
				desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_WCOUNT ... OUTPTYPE_WSTD:
		dbwealth.get_sday(desc->item - OUTPITEM_PRICE + WEALTH_ITEM_PRICE,
				desc->type - OUTPTYPE_WCOUNT + WEALTH_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_WP ... OUTPTYPE_WMF:
		dbwealth.get_trends(desc->type - OUTPTYPE_WP + WEALTH_TRENDS_P,
				desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_WSMA ... OUTPTYPE_WOBV:
		dbwealth.get_trends_acum(desc->type - OUTPTYPE_WSMA + WEALTH_TRENDS_ACUM_SMA,
				desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_IMIN ... OUTPTYPE_RMAX:
		dbinterpolator.get_results(desc->value.c_str(), desc->type - OUTPTYPE_IMIN + INTERPT_MIN5,
				desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_EIMIN ... OUTPTYPE_ERMAX:
		dbinterpolator.get_equations(desc->value.c_str(), desc->type - OUTPTYPE_EIMIN + INTERPT_MIN5,
				desc->day_start, desc->day_end,
				data, &times[0], &times[1]);
		break;
	}
}

void Output::merge_data(const std::vector<int> times[2], const std::vector<double> *data,
		std::vector<int> t[2], std::vector<std::vector<double> > *X)
{
	DLOG("Output::merge_data(before)-> t[0].size: %d, t[1].size: %d, X->size: %d", t[0].size(), t[1].size(), X->size());
	if (X->empty())
	{
		t[0] = times[0];
		t[1] = times[1];
		X->push_back(*data);
		if (config.normalize >= 0)
		{
			normax = 0.0;
			for (int i = 0; i < data->size(); i++)
				if (normax < fabs(data->at(i))) normax = fabs(data->at(i));
		}
		if (config.normalize > 0)
		{
			double K = config.normalize / normax;
			for (int i = 0; i < X->front().size(); i++)
				X->front()[i] *= K;
			normax = config.normalize;
		}
		
	}
	else if (data->size())
	{
		std::vector<int> tr[2];
		std::vector<std::vector<double> > Xr;
		Xr.resize(X->size() + 1);
		for (int i = 0, j = 0; i < times[0].size() || j < t[0].size(); )
		{
#define MERGE_STEP(_t, _i) do { \
	tr[0].push_back(_t[0][_i]); \
	if (_i < _t[1].size()) \
		tr[1].push_back(_t[1][_i]); \
	else if (tr[1].size()) \
		tr[1].push_back(tr[1].back()); \
	for (int k = 0; k < X->size(); k++) \
		Xr[k].push_back(j < X->at(k).size() ? X->at(k).at(j) : X->at(k).back()); \
	Xr[X->size()].push_back(i < data->size() ? data->at(i) : data->back()); \
	_i++; } while (0)

			if (i >= times[0].size())
			{
				MERGE_STEP(t, j);
			}
			else if (j >= t[0].size())
			{
				MERGE_STEP(times, i);
			}
			else
			{
				if (t[0][j] < times[0][i])
				{
					MERGE_STEP(t, j);
				}
				else if (times[0][i] < t[0][j])
				{
					MERGE_STEP(times, i);
				}
				else
				{
					if (j >= t[1].size() && i < times[1].size())
					{
						MERGE_STEP(times, i);
						//j++;
					}
					else if (i >= times[1].size())
					{
						MERGE_STEP(t, j);
						if (j >= t[1].size()) i++;
					}
					else //if (j < t[1].size() && i < times[1].size())
					{
						if (t[1][j] < times[1][i])
						{
							MERGE_STEP(t, j);
						}
						else if (times[1][i] < t[1][j])
						{
							MERGE_STEP(times, i);
						}
						else
						{
							MERGE_STEP(t, j);
							i++;
						}
					}
				}
			}
		}
		if (config.normalize >= 0)
		{
			double K = 0.0;
			for (int i = 0; i < Xr[X->size()].size(); i++)
				if (K < fabs(Xr[X->size()][i])) K = fabs(fabs(Xr[X->size()][i]));
			K = normax / K;
			for (int i = 0; i < Xr[X->size()].size(); i++)
				Xr[X->size()][i] *= K;
		}
		t[0] = tr[0];
		t[1] = tr[1];
		*X = Xr;
	}
	DLOG("Output::merge_data(after)-> t[0].size: %d, t[1].size: %d, X->size: %d", t[0].size(), t[1].size(), X->size());
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
		std::vector<int> t[2];
		std::vector<std::vector<double> > X;
		if (!config.outpdescs.empty())
		{
			for (int i = 0; i < config.outpdescs.size(); i++)
			{
				std::vector<double> data;
				std::vector<int> times[2];
				get_data(&config.outpdescs[i], &data, times);
				merge_data(times, &data, t, &X);
			}
		}
		else
		{
			X.resize(1);
			get_data(&config.outpdesc, &X[0], t);
		}
		front->output(t, &X);
	}
}
