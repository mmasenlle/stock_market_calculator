
#include "logger.h"
#include "CcltorObservers.h"

void CcltorObservers::add(const ICPeer *observer, const ICRegistration *reg)
{
	DLOG("CcltorICObservers::add(0x%08x:%d, %d)", observer->get_ip(), observer->get_port(), reg->getSeconds());
	observers[*observer] = reg->getSeconds() > 0 ? (time(NULL) + reg->getSeconds()) : 0;
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
			SELOG("CcltorICObservers::notify() -> sending to 0x%08x:%d",
					safe->first.get_ip(), safe->first.get_port());
		}
	}
}
