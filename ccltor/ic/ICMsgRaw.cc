
#include "string.h"
#include "ICMsgRaw.h"

ICMsgRaw::ICMsgRaw() : ICMsg(ICMSGCLASS_RAW), size(0), data(NULL), mem(NULL) {}

ICMsgRaw::ICMsgRaw(const char *msg, int len) : ICMsg(ICMSGCLASS_RAW), size(len)
{
	mem = new char[size];
	data = mem;
	memcpy(data, msg, size);
}

ICMsgRaw::~ICMsgRaw()
{
	delete [] mem;
}

int ICMsgRaw::send(const CcltorIC *ic, const ICPeer *peer) const
{
	char *buffer = new char[size + SIZEOF_ICMSGCLASS];
	memcpy(buffer + SIZEOF_ICMSGCLASS, data, size);
	int r = send_msg(ic, peer, buffer, size + SIZEOF_ICMSGCLASS);
	delete [] buffer;
	return r;
}
