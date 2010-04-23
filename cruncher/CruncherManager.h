#ifndef _CRUNCHERMANAGER_H_
#define _CRUNCHERMANAGER_H_

#include <map>
#include <set>
#include <pthread.h>
#include "CcltorIC.h"
#include "CruncherConfig.h"
#include "ICruncher.h"

struct Cruncher
{
	char stack[32 * 1024];
	ICruncher *cruncher;
	int pid;
	pthread_mutex_t mtx;
};

class CruncherManager : public ICruncherManager
{
	CruncherConfig config;
	CcltorIC ic;
	
	pthread_mutex_t manager_mtx;
	std::map<int, Cruncher *> crunchers;
	std::map<int, std::set<int> > observers;
	
	void handleIC();
	
	static int cruncher_fn(void *);

public:
	CruncherManager();

	int observe(int event);
	int send(ICMsg *msg, const ICPeer *peer);

	void init();
	void run();
};

#endif
