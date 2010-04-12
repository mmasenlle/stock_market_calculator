
#include "logger.h"
#include "CcltorObservers.h"

void CcltorObservers::add(const ICPeer *observer, int seconds)
{
	observers[*observer] = seconds > 0 ? (time(NULL) + seconds) : 0;
}

void CcltorObservers::notify(const CcltorIC *ic, const ICMsg *msg)
{
	time_t tt = time(NULL);
	std::map<ICPeer, time_t>::iterator i = observers.begin();
	while (i != observers.end())
	{
		std::map<ICPeer, time_t>::iterator safe = i++;
		if (safe->second && tt > safe->second)
		{
			DLOG("CcltorObservers::notify() -> dropping observer %d>%d", tt, safe->second);
			observers.erase(safe);
		}
		else if (msg->send(ic, &safe->first) < 0)
		{
			SELOG("CcltorICObservers::notify() -> sending");
		}
	}
}
