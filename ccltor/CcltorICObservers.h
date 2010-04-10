#ifndef _CCLTORICOBSERVERS_H_
#define _CCLTORICOBSERVERS_H_

#include <set>
#include "CcltorIC.h"


class CcltorICObservers
{
	std::set<CcltorICPeer> observers;

public:
	void add(const CcltorICPeer *observer);
	void notify(const CcltorIC *ic, CcltorICMsg *msg, int len);
};

#endif
