#ifndef ICEVENT2_H_
#define ICEVENT2_H_

#include "ICEvent.h"

enum
{
	ICEVENT2_CONTROL_RUNNING_PING,
	ICEVENT2_CONTROL_RUNNING_STOP,
	ICEVENT2_CONTROL_RUNNING_RECONNECT,

};

enum
{
	ICEVENT2_CONTROL_LOGLEVEL_NONE,
	ICEVENT2_CONTROL_LOGLEVEL_SYSERR,
	ICEVENT2_CONTROL_LOGLEVEL_ERROR,
	ICEVENT2_CONTROL_LOGLEVEL_WARN,
	ICEVENT2_CONTROL_LOGLEVEL_INFO,
	ICEVENT2_CONTROL_LOGLEVEL_ALL
};

#define SIZEOF_ICEVENT2 (sizeof(uint32_t) + SIZEOF_ICEVENT)

class ICEvent2 : public ICEvent
{
	int sub_event;
protected:
	void putSubEvent(char *buf) const;
public:
	ICEvent2(int ev, int s_ev) : ICEvent(ev), sub_event(s_ev) {}
	int getSubEvent() { return sub_event; };
	virtual int send(const CcltorIC *ic, const ICPeer *peer) const;
	static ICEvent2 *create(int event, const char *msg, int len);
};

#endif /*ICEVENT2_H_*/
