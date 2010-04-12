#ifndef _CCLTOROBSERVERS_H_
#define _CCLTOROBSERVERS_H_

#include <time.h>
#include <map>
#include "CcltorIC.h"


class CcltorObservers
{
	std::map<ICPeer, time_t> observers;

public:
	void add(const ICPeer *observer, int seconds);
	void notify(const CcltorIC *ic, const ICMsg *msg);
};

#endif
