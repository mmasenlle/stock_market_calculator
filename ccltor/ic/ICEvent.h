#ifndef ICEVENT_H_
#define ICEVENT_H_

#include "ICMsg.h"

enum
{
	ICEVENT_EVENT = 0,
	ICEVENT_FEEDER_NEWFEED,
	ICEVENT_STATISTICS_UPDATED,
	ICEVENT_TRENDS_UPDATED,
	
	ICEVENT_EVENT2,
	ICEVENT_CONTROL_RUNNING,
	ICEVENT_CONTROL_LOGLEVEL,
	
};

#define SIZEOF_ICEVENT (sizeof(uint32_t) + SIZEOF_ICMSGCLASS)

class ICEvent : public ICMsg
{
	int event;
protected:
	void putEvent(char *buf) const;
public:
	ICEvent(int ev) : ICMsg(ICMSGCLASS_EVENT), event(ev) {}
	int getEvent() { return event; };
	virtual int send(const CcltorIC *ic, const ICPeer *peer) const;
	static ICEvent *create(const char *msg, int len);
};

#endif /*ICEVENT_H_*/
