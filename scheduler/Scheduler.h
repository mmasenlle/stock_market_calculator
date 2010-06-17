#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <time.h>
#include "CcltorIC.h"
#include "SchedulerConfig.h"

class Scheduler
{
	int next;
	int delay;
	time_t next_time;
	std::vector<time_t> last_stamps;
	
	CcltorIC ic;
	
	void handle_msg(ICMsg *msg, ICPeer *from);
	void update();
	void exec();

public:
	SchedulerConfig config;
	
	Scheduler();
	void init();
	void run();
};

#endif
