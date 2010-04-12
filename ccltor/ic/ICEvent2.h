#ifndef ICEVENT2_H_
#define ICEVENT2_H_

#include "ICEvent.h"

enum
{
	ICEVENT2_SUBEVENT = 0,

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
