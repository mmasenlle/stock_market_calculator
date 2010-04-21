
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

int Statistics::init(ICruncherManager *icm)
{
	manager = icm;
	if (db.connect(CruncherConfig::config.db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Statistics::init() -> db.connect(%s)", CruncherConfig::config.db_conninfo.c_str());
		return -1;
	}

	return 0;
}

int Statistics::run()
{
	manager->observe(ICEVENT_FEEDER_NEWFEED);
	// get last calculated data
	for (;;)
	{
	//   get data to proccess
	//   if (there is new data)
	//       process
	//   else
		{	//       wait event and continue
			pthread_mutex_lock(&mtx);
			while (!newfeeds)
				pthread_cond_wait(&cond, &mtx);
			newfeeds = 0;
			pthread_mutex_unlock(&mtx);
			continue;
		}
	//   compute data
	//   store data
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
