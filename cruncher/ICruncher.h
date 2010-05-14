#ifndef _ICRUNCHER_H_
#define _ICRUNCHER_H_

class CruncherConfig;
class DBCache;
class ICMsg;
class ICPeer;

struct ICruncherManager
{
	CruncherConfig *ccfg;
	DBCache *cache;
	virtual int observe(int event) = 0;
	virtual int send(ICMsg *msg, const ICPeer *peer) = 0;
};

class Logger;

struct ICruncher
{
	virtual int init(ICruncherManager *manager) = 0;
	virtual int run() = 0;
	virtual int msg(ICMsg *msg) = 0;
	virtual ~ICruncher() {};
};

typedef ICruncher * (*getInstance_t)();
#define CRUNCHER_GETINSTANCE getInstance
#define CRUNCHER_GETINSTANCE_SYMBOL "getInstance"

#endif
