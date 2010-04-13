#ifndef ICMSG_H_
#define ICMSG_H_

#include <stdlib.h>
#include <stdint.h>

enum
{
	ICMSGCLASS_RAW = 0,
	ICMSGCLASS_EVENT,
	ICMSGCLASS_REGISTER,
};

#define SIZEOF_ICMSGCLASS sizeof(uint32_t)

class CcltorIC;
class ICPeer;

class ICMsg
{
	int mclass;
protected:
	ICMsg(int mc) : mclass(mc) {};
	int send_msg(const CcltorIC *ic, const ICPeer *peer, char *buf, int len) const;
public:
	virtual ~ICMsg() {};
	int getClass() const { return mclass; };
	virtual int send(const CcltorIC *ic, const ICPeer *peer) const = 0;
	static ICMsg *create(const char *msg, int len);
};

#endif /*ICMSG_H_*/
