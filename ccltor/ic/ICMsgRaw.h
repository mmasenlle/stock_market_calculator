#ifndef ICMSGRAW_H_
#define ICMSGRAW_H_

#include "ICMsg.h"

class ICMsgRaw : public ICMsg
{
public:
	int size;
	char *data;
	char *mem;

	ICMsgRaw();
	ICMsgRaw(const char *msg, int len);
	~ICMsgRaw();
	int send(const CcltorIC *ic, const ICPeer *peer) const;
};

#endif /*ICMSGRAW_H_*/
