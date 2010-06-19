#ifndef _FEEDER_H_
#define _FEEDER_H_

#include <sys/time.h>
#include <poll.h>
#include "ICEvent.h"
#include "CcltorObservers.h"
#include "DBfeeder.h"
#include "FeederConfig.h"

class Feeder
{
	CcltorDB db;

	ICEvent newfeed;
	CcltorIC ic;
	CcltorObservers obs;

	int pid_wget;
	struct timeval last_feed;
	struct pollfd pfds[1];

	void handleEvent(ICEvent *event, ICPeer *peer);
	void handleIC();
	int feed();
	
	static void sigchld_handler(int s);
	
public:
	FeederConfig config;
	DBfeeder dbfeeder;

	Feeder();
	void init();
	void run();
	
	static Feeder feeder;
};

#endif /*FEEDER_H_*/
