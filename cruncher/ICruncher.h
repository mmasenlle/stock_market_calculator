#ifndef ICRUNCHER_H_
#define ICRUNCHER_H_

class ICMsg;
class ICPeer;

struct ICruncherManager
{
	virtual const char *get_db_conninfo() = 0;
	virtual int observe(int event) = 0;
	virtual int send(ICMsg *msg, const ICPeer *peer) = 0;
};

class Logger;

struct ICruncher
{
	virtual int init(ICruncherManager *manager, Logger *logger) = 0;
	virtual int run() = 0;
	virtual int msg(ICMsg *msg) = 0;
	virtual ~ICruncher() {};
};

typedef ICruncher * (*getInstance_t)();
#define CRUNCHER_GETINSTANCE getInstance
#define CRUNCHER_GETINSTANCE_SYMBOL "getInstance"

#endif /*ICRUNCHER_H_*/
