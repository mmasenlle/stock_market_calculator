
#include <map>
#include <float.h>
#include <math.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent.h"
#include "CruncherConfig.h"
#include "Statistics.h"

extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new Statistics;
}

Statistics::Statistics() : dbfeeder(&db), dbstatistics(&db)
{
	newfeeds = 0;
	int r = pthread_mutex_init(&mtx, NULL);
	if (r != 0 || (r = pthread_cond_init(&cond, NULL)) != 0)
	{
		ELOG("Statistics::Statistics() -> pthread_{mutex,cond}_init(): %d", r);
	}
	force_until = 0;
}

Statistics::~Statistics()
{
	int r = pthread_cond_destroy(&cond);
	if (r != 0 || (r = pthread_mutex_destroy(&mtx)) != 0)
	{
		ELOG("Statistics::~Statistics() -> pthread_{mutex,cond}_destroy(): %d", r);
	}
}

int Statistics::init(ICruncherManager *icm)
{
	manager = icm;
	if (db.connect(manager->ccfg->db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Statistics::init() -> db.connect(%s)", manager->ccfg->db_conninfo.c_str());
		return -1;
	}
	force_until = manager->ccfg->force_until;
	return 0;
}

int Statistics::calculate(const std::vector<double> *data,
			double *min, double *mean, double *max, double *std)
{
	*min = DBL_MAX;
	*mean = 0.0;
	*max = DBL_MIN;
	for (int j = 0; j < data->size(); j++)
	{
		if (*min > data->at(j)) *min = data->at(j);
		*mean += data->at(j);
		if (*max < data->at(j)) *max = data->at(j);
	}
	*std = 0.0;
	if (data->size() > 1)
	{
		*mean /= data->size();
		for (int j = 0; j < data->size(); j++)
		{
			*std += ((data->at(j) - *mean) * (data->at(j) - *mean));
		}
		*std /= (data->size() - 1);
		*std = sqrt(*std);
	}
	return data->size();
}

void Statistics::calculate_days(const char *cod, int start)
{
	DLOG("Statistics::calculate_days(%s, %d)", cod, start);
	int empty_days = 0;
	for (int day = start; force_until < day; day = utils::dec_day(day))
	{
		bool some_data = false;
		double r[3][5];
		std::vector<double> v[3];
		for (int j = 0; j < 3; j++)
		{
			dbfeeder.get_value_data(cod, j, day, 0, day, 0, &v[j], NULL, NULL);
			r[j][0] = calculate(&v[j], &r[j][1], &r[j][2], &r[j][3], &r[j][4]);
			some_data = some_data || (r[j][0] > 0.0);
		}
		if (!force_until && !some_data)
		{
			if (++empty_days > 15)
			{
				DLOG("Statistics::calculate_days(%s) -> %08d empty_days: %d, returning ...", cod, day, empty_days);
				return;
			}
			continue;
		}
		empty_days = 0;
		bool the_same = true;
		for (int j = 0; the_same && j < 3; j++)
		{
			for (int k = 0; the_same && k < 5; k++)
			{
				std::vector<double> d;
				dbstatistics.get_day(cod, j, k, day, day, &d, NULL);
				the_same = (d.size() == 1 && r[j][k] == d[0]);
			}
		}
		if (!force_until && the_same)
		{
			DLOG("Statistics::calculate_days(%s) -> %08d same data, returning ...", cod, day);
			return;
		}
		ILOG("Statistics::calculate_days(%s) -> insert_day %08d", cod, day);
		dbstatistics.insert_day(cod, day, r[0][0], r[1][0], r[2][0],
				r[0][1], r[1][1], r[2][1], r[0][2], r[1][2], r[2][2],
				r[0][3], r[1][3], r[2][3], r[0][4], r[1][4], r[2][4]);
	}
}

double Statistics::meta_calculate(const std::vector<double> *data, int what)
{
	double r = 0.0;
	switch (what)
	{
	case 0: for (int i = 0; i < data->size(); i++) r += data->at(i); break;
	case 1: r = DBL_MAX; for (int i = 0; i < data->size(); i++) if (r > data->at(i)) r = data->at(i); break;
	case 3: r = DBL_MIN; for (int i = 0; i < data->size(); i++) if (r < data->at(i)) r = data->at(i); break;
	default: for (int i = 0; i < data->size(); i++) r += data->at(i); r/= data->size(); break;
	}
	return r;
}

void Statistics::calculate_months(const char *cod, int start)
{
	DLOG("Statistics::calculate_months(%s, %d)", cod, start);
	int last_mday = start;
	int first_mday = ((last_mday / 100) * 100) + 1;
	while (force_until < last_mday)
	{
		bool some_data = false;
		double r[3][5];
		std::vector<double> v[3][5];
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 5; k++)
			{
				dbstatistics.get_day(cod, j, k, first_mday, last_mday, &v[j][k], NULL);
				r[j][k] = 0.0;
				if (v[j][k].size())
				{
					r[j][k] = meta_calculate(&v[j][k], k);
					some_data = true;
				}
			}
		}
		if (!force_until && !some_data)
		{
			DLOG("Statistics::calculate_months(%s) -> %08d no data, returning ...", cod, first_mday);
			return;
		}
		bool the_same = true;
		for (int j = 0; the_same && j < 3; j++)
		{
			for (int k = 0; the_same && k < 5; k++)
			{
				std::vector<double> d;
				dbstatistics.get_month(cod, j, k, first_mday, first_mday, &d, NULL);
				the_same = (d.size() == 1 && r[j][k] == d[0]);
			}
		}
		if (!force_until && the_same)
		{
			DLOG("Statistics::calculate_months(%s) -> %08d same data, returning ...", cod, first_mday);
			return;
		}
		ILOG("Statistics::calculate_months(%s) -> insert_month %08d", cod, first_mday);
		dbstatistics.insert_month(cod, first_mday, r[0][0], r[1][0], r[2][0],
				r[0][1], r[1][1], r[2][1], r[0][2], r[1][2], r[2][2],
				r[0][3], r[1][3], r[2][3], r[0][4], r[1][4], r[2][4]);
		last_mday = utils::dec_day(first_mday);
		first_mday = ((last_mday / 100) * 100) + 1;
	}
}

void Statistics::calculate_years(const char *cod, int start)
{
	DLOG("Statistics::calculate_years(%s, %d)", cod, start);
	int last_yday = start;
	int first_yday = ((last_yday / 10000) * 10000) + 101;
	while (force_until < last_yday)
	{
		bool some_data = false;
		double r[3][5];
		std::vector<double> v[3][5];
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 5; k++)
			{
				dbstatistics.get_month(cod, j, k, first_yday, last_yday, &v[j][k], NULL);
				r[j][k] = 0.0;
				if (v[j][k].size())
				{
					r[j][k] = meta_calculate(&v[j][k], k);
					some_data = true;
				}
			}
		}
		if (!force_until && !some_data)
		{
			DLOG("Statistics::calculate_years(%s) -> %08d no data, returning ...", cod, first_yday);
			return;
		}
		bool the_same = true;
		for (int j = 0; the_same && j < 3; j++)
		{
			for (int k = 0; the_same && k < 5; k++)
			{
				std::vector<double> d;
				dbstatistics.get_year(cod, j, k, first_yday, first_yday, &d, NULL);
				the_same = (d.size() == 1 && r[j][k] == d[0]);
			}
		}
		if (!force_until && the_same)
		{
			DLOG("Statistics::calculate_years(%s) -> %08d same data, returning ...", cod, first_yday);
			return;
		}
		ILOG("Statistics::calculate_years(%s) -> insert_year %08d", cod, first_yday);
		dbstatistics.insert_year(cod, first_yday, r[0][0], r[1][0], r[2][0],
				r[0][1], r[1][1], r[2][1], r[0][2], r[1][2], r[2][2],
				r[0][3], r[1][3], r[2][3], r[0][4], r[1][4], r[2][4]);
		last_yday = utils::dec_day(first_yday);
		first_yday = ((last_yday / 100) * 100) + 1;
	}
}

int Statistics::run()
{
	manager->observe(ICEVENT_FEEDER_NEWFEED);
	std::map<std::string, std::pair<int, int> > last_stamps;
	
	for (;;)
	{
		std::vector<std::string> codes;
		dbfeeder.get_value_codes(&codes);
		for (int i = 0; i < codes.size(); i++)
		{
			int recentst_day = utils::today(), recentst_time = 0;
			for (int j = 0; force_until < recentst_day; j++)
			{
				std::vector<int> dates, times;
				dbfeeder.get_value_data(codes[i].c_str(), 0, recentst_day, recentst_time, 0, 0,
				    NULL, &dates, &times);
				if (dates.size() && dates.size() == times.size())
				{
					recentst_day = dates[dates.size() - 1];
					recentst_time = times[times.size() - 1];
					break;
				}
				if (!force_until && j > 25)
				{
					WLOG("Statistics::run(%s) -> can't find recentst day");
					break;
				}	
				recentst_day = utils::dec_day(recentst_day);
			}
			if (last_stamps.find(codes[i]) != last_stamps.end())
			{
				if (last_stamps[codes[i]].first == recentst_day &&
						last_stamps[codes[i]].second == recentst_time)
					continue;
			}
			calculate_days(codes[i].c_str(), recentst_day);
			calculate_months(codes[i].c_str(), recentst_day);
			calculate_years(codes[i].c_str(), recentst_day);

			last_stamps[codes[i]].first = recentst_day;
			last_stamps[codes[i]].second = recentst_time;
		}
		ILOG("Statistics::run(%s) -> done for now (last_stamps.size(): %d, newfeeds: %d)",
				last_stamps.size(), newfeeds);
		pthread_mutex_lock(&mtx);
		while (!newfeeds)
			pthread_cond_wait(&cond, &mtx);
		newfeeds = 0;
		pthread_mutex_unlock(&mtx);
	} 

	return 0;
}

int Statistics::msg(ICMsg *msg)
{
	if (msg->getClass() == ICMSGCLASS_EVENT && ((ICEvent*)msg)->getEvent() == ICEVENT_FEEDER_NEWFEED)
	{
		pthread_mutex_lock(&mtx);
		newfeeds++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
		ILOG("Statistics::msg() -> ICEVENT_FEEDER_NEWFEED (%d)", newfeeds);
	}
	return 0;
}
