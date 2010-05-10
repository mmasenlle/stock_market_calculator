
#include "logger.h"
#include "Table.h"
#include "Chart.h"
#include "Output.h"

Output::Output() : dbfeeder(&db), dbstatistics(&db), dbtrends(&db)
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
	case OUTPTYPE_ALL: case	OUTPTYPE_OPEN: case OUTPTYPE_CLOSE:
		dbfeeder.get_value_data(desc->value.c_str(), desc->item - OUTPITEM_PRICE + FEEDER_DATAITEM_PRICE,
				desc->day_start, desc->time_start, desc->day_end, desc->time_end,
				data, &times[0], &times[1]);
		if (desc->type != OUTPTYPE_ALL)
		{
			std::vector<double> ldata; std::vector<int> ltimes[2];
			if (desc->type == OUTPTYPE_OPEN)
			{
				for (int i = 0; i < data->size(); i++)
				{
					if (!ltimes[0].size() || times[0].at(i) != ltimes[0].back())
					{
						ldata.push_back(data->at(i));
						ltimes[0].push_back(times[0].at(i));
						ltimes[1].push_back(times[1].at(i));
					}
				}
			}
			else if (desc->type == OUTPTYPE_CLOSE)
			{
				double v; int d = times[0].size() ? times[0].front() : 0, t;
				for (int i = 0; i < data->size(); i++)
				{
					if (times[0].at(i) != d)
					{
						ldata.push_back(v);
						ltimes[0].push_back(d);
						ltimes[1].push_back(t);
					}
					v = data->at(i);
					d = times[0].at(i);
					t = times[1].at(i);
				}
			}
			*data = ldata;
			times[0] = ltimes[0];
			times[1] = ltimes[1];
		}
		break;
	case OUTPTYPE_COUNT: case OUTPTYPE_MIN: case OUTPTYPE_MEAN: case OUTPTYPE_MAX: case OUTPTYPE_STD:
		dbstatistics.get_day(desc->value.c_str(), desc->item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				desc->type - OUTPTYPE_COUNT + STATISTICS_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_MCOUNT: case OUTPTYPE_MMIN: case OUTPTYPE_MMEAN: case OUTPTYPE_MMAX: case OUTPTYPE_MSTD:
		dbstatistics.get_month(desc->value.c_str(), desc->item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				desc->type - OUTPTYPE_MCOUNT + STATISTICS_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_YCOUNT: case OUTPTYPE_YMIN: case OUTPTYPE_YMEAN: case OUTPTYPE_YMAX: case OUTPTYPE_YSTD:
		dbstatistics.get_year(desc->value.c_str(), desc->item - OUTPITEM_PRICE + STATISTICS_ITEM_PRICE,
				desc->type - OUTPTYPE_YCOUNT + STATISTICS_STC_COUNT, desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_P ... OUTPTYPE_MF:
		dbtrends.get(desc->value.c_str(), desc->type - OUTPTYPE_P + TRENDS_P,
				desc->day_start, desc->day_end,
				data, &times[0]);
		break;
	case OUTPTYPE_SMA ... OUTPTYPE_MFI:
		dbtrends.get_acum(desc->value.c_str(), desc->type - OUTPTYPE_SMA + TRENDS_ACUM_SMA,
				desc->day_start, desc->day_end,
				data, &times[0]);
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
						j++;
					}
					else if (i >= times[1].size())
					{
						MERGE_STEP(t, j);
						i++;
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
