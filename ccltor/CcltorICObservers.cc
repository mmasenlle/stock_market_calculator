
#include "logger.h"
#include "CcltorICObservers.h"

void CcltorICObservers::add(const CcltorICPeer *observer)
{
	observers.insert(*observer);
}

void CcltorICObservers::notify(const CcltorIC *ic, CcltorICMsg *msg, int len)
{
	for (std::set<CcltorICPeer>::iterator i = observers.begin(); i != observers.end(); i++)
	{
		if (ic->send(&*i, msg, len) != len)
		{
			SELOG("CcltorICObservers::notify() -> sending (%d)", len);
		}
	}
}
