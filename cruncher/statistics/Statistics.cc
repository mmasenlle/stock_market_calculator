
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
}

Statistics::~Statistics()
{
	int r = pthread_cond_destroy(&cond);
	if (r != 0 || (r = pthread_mutex_destroy(&mtx)) != 0)
	{
		ELOG("Statistics::~Statistics() -> pthread_{mutex,cond}_destroy(): %d", r);
	}
}

int Statistics::init(ICruncherManager *icm, Logger *logger)
{
//	Logger::defaultLogger = logger;
	manager = icm;
	if (db.connect(manager->get_db_conninfo()) != CONNECTION_OK)
	{
		ELOG("Statistics::init() -> db.connect(%s)", manager->get_db_conninfo());
		return -1;
	}

	return 0;
}

void Statistics::calculate(const std::vector<double> *data,
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
	*mean /= data->size();
	*std = 0.0;
	if (data->size() > 1)
	{
		for (int j = 0; j < data->size(); j++)
		{
			*std += ((data->at(j) - *mean) * (data->at(j) - *mean));
		}
		*std /= (data->size() - 1);
		*std = sqrt(*std);
	}
}

void Statistics::calculate_days(const char *cod, int start)
{
	DLOG("Statistics::calculate_days(%s, %d)", cod, start);
	int empty_days = 0;
	for (int day = start;; day = utils::dec_day(day))
	{
		std::vector<double> data[3];
		dbfeeder.get_value_prices(cod, day, 0, day, 235959, &data[0], NULL, NULL);
		dbfeeder.get_value_volumes(cod, day, 0, day, 235959, &data[1], NULL, NULL);
		dbfeeder.get_value_capitals(cod, day, 0, day, 235959, &data[2], NULL, NULL);
		if (data[0].size() != data[1].size() || data[1].size() != data[2].size())
		{
			WLOG("Statistics::calculate_days(%s) -> %08d data sizes mismatch %d %d %d", cod, day,
					data[0].size(), data[1].size(), data[1].size(), data[2].size());
			continue;
		}
		if (!data[0].size())
		{
			DLOG("Statistics::calculate_days(%s) -> %08d no data, empty_days: %d", cod, day, empty_days);
			if (++empty_days > 15)
				break;
			continue;
		}
		empty_days = 0;
		int count[3][2] = { data[0].size(), 0, data[1].size(), 0, data[2].size(), 0 };
		double v[4][3][2];
		for (int j = 0; j < 3; j++)
			calculate(&data[j], &v[0][j][0], &v[1][j][0], &v[2][j][0], &v[3][j][0]);
		if (day != start)
		{
			dbstatistics.get_day(cod, day, &count[0][1], &count[1][1], &count[2][1],
					&v[0][0][0], &v[0][1][0], &v[0][2][0], &v[1][0][0], &v[1][1][0], &v[1][2][0],
					&v[2][0][0], &v[2][1][0], &v[2][2][0], &v[3][0][0], &v[3][1][0], &v[3][2][0]);
			bool the_same = true;
			for (int j = 0; the_same && j < 3; j++)
				the_same = (count[j][0] == count[j][1]);
			for (int k = 0; the_same && k < 4; k++)
				for (int j = 0; the_same && j < 3; j++)
					the_same = (v[k][j][0] == v[k][j][1]);
			if (the_same)
				break;
		}
		ILOG("Statistics::calculate_days(%s) -> insert_day %08d", cod, day);
		dbstatistics.insert_day(cod, day, count[0][0], count[1][0], count[2][0],
				v[0][0][0], v[0][1][0], v[0][2][0], v[1][0][0], v[1][1][0], v[1][2][0],
				v[2][0][0], v[2][1][0], v[2][2][0], v[3][0][0], v[3][1][0], v[3][2][0]);
	}
}

void Statistics::calculate_months(const char *cod, int start)
{
	DLOG("Statistics::calculate_months(%s, %d)", cod, start);
	int empty_days = 0;
	int day = start;
	int month = day / 100;
	std::vector<int> counts[3];
	std::vector<double> v[12];
	int j = 0;
	for (;;)
	{
		if (month != (day / 100))
		{
			int count[3][2] = { 0, 0, 0, 0, 0, 0 };
			double r[12][2] = { DBL_MAX, .0, DBL_MAX, .0, DBL_MAX, .0, .0, .0, .0, .0, .0, .0,
					DBL_MIN, .0, DBL_MIN, .0, DBL_MIN, .0, .0, .0, .0, .0, .0, .0 };
			for (int i = 0; i < j; i++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (counts[k][j])
					{
						count[k][0] += counts[k][j];
						if (r[k][0] > v[k][j]) r[k][0] = v[k][j];
						r[3 + k][0] += (v[3 + k][j] * counts[k][j]);
						if (r[6 + k][0] > v[6 + k][j]) r[6 + k][0] = v[6 + k][j];
						r[9 + k][0] += (v[9 + k][j] * counts[k][j]);
						empty_days = 0;
					}
					else
					{
						empty_days++;
					}
				}
			}
			for (int k = 0; k < 3; k++)
			{
				if (count[k][0])
				{
					r[3 + k][0] /= count[k][0];
					r[9 + k][0] /= count[k][0];
				}
			}
			int mday = (month * 100) + 1;
			dbstatistics.get_month(cod, mday, &count[0][1], &count[1][1], &count[2][1],
										&r[0][1], &r[1][1], &r[2][1], &r[3][1], &r[4][1], &r[5][1],
										&r[6][1], &r[7][1], &r[8][1], &r[9][1], &r[10][1], &r[11][1]);
			bool the_same = true;
			for (int i = 0; the_same && i < 3; j++)
				the_same = (count[i][0] == count[i][1]);
			for (int i = 0; the_same && i < 12; j++)
				the_same = (r[i][0] == r[i][1]);
			if (the_same)
				return;
			ILOG("Statistics::calculate_months(%s) -> insert_month %08d", cod, mday);
			dbstatistics.insert_month(cod, mday, count[0][0], count[1][0], count[2][0],
										r[0][0], r[1][0], r[2][0], r[3][0], r[4][0], r[5][0],
										r[6][0], r[7][0], r[8][0], r[9][0], r[10][0], r[11][0]);
			if (empty_days > 30)
				return;
			for (int i = 0; i < ARRAY_SIZE(counts); i++) counts[i].clear();
			for (int i = 0; i < ARRAY_SIZE(v); i++) v[i].clear();
			month = day / 100;
			j = 0;
		}
		for (int i = 0; i < ARRAY_SIZE(counts); i++) counts[i].push_back(0);
		for (int i = 0; i < ARRAY_SIZE(v); i++) v[i].push_back(0.0);
		dbstatistics.get_day(cod, day, &counts[0][j], &counts[1][j], &counts[2][j],
							&v[0][j], &v[1][j], &v[2][j], &v[3][j], &v[4][j], &v[5][j],
							&v[6][j], &v[7][j], &v[8][j], &v[9][j], &v[10][j], &v[11][j]);
		j++;
	}
}

void Statistics::calculate_years(const char *cod, int start)
{
	DLOG("Statistics::calculate_years(%s, %d)", cod, start);
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
			for (int j = 0; j < 25; j++)
			{
				std::vector<int> dates, times;
				dbfeeder.get_value_prices(codes[i].c_str(), recentst_day, recentst_time, 20500101, 1, NULL,
						&dates, &times);
				if (dates.size() && dates.size() == times.size())
				{
					recentst_day = dates[dates.size() - 1];
					recentst_time = times[times.size() - 1];
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
	
	/*
	retrieve all values
	 for each value
	   if is in cache
	      retrieve prices from timestamp until now
	      if not new
	         break;

	   day = today
	   retrieve day data
	   calculate day statistics
	   store day statistics
	   for
	   		day--
	        retrieve day statistics
	        retrieve day data
	        calculate day statistics
	        if (calculated == stored)
	           break
	        store day statistics
	        
	   month = this month
	   retrieve day statistics
	   calculate month statistics
	   store month statistics
	   for
	   		month--
	   		retrieve month statistics
	   		retrieve day statistics
	   		calculate month statistics
	   		if (calculated == stored)
	           break
	        store month statistics
	        	        
	   year = this year
	   retrieve month statistics
	   calculate year statistics
	   store year statistics
	   for
	   		year--
	   		retrieve year statistics
	   		retrieve month statistics
	   		calculate year statistics
	   		if (calculated == stored)
	           break
	        store year statistics
	        
	   cache value and last timestamp of feeder
	
	
	*/

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
