#ifndef _CRUNCHERMANAGER_H_
#define _CRUNCHERMANAGER_H_

#include <map>
#include <set>
#include <pthread.h>
#include "CcltorIC.h"
#include "DBCache.h"
#include "CruncherConfig.h"
#include "ICruncher.h"

struct Cruncher
{
	ICruncher *cruncher;
	pthread_t tid;
	pthread_mutex_t mtx;
};

class CruncherManager : public ICruncherManager
{
	CruncherConfig config;
	CcltorIC ic;
	DBCache dbcache;
	
	pthread_mutex_t manager_mtx;
	std::map<pthread_t, Cruncher *> crunchers;
	std::map<int, std::set<pthread_t> > observers;
	
	void handle_msg(ICMsg *msg, ICPeer *from);
	void handle_ic();
	
	static void *cruncher_fn(void *);

public:
	CruncherManager();

	int observe(int event);
	int send(ICMsg *msg, const ICPeer *peer);

	void init();
	void run();
};

#endif
