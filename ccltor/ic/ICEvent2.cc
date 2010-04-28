
#include <arpa/inet.h>
#include "ICEvent2.h"

void ICEvent2::putSubEvent(char *buf) const
{
	*(uint32_t*)(buf + SIZEOF_ICEVENT) = htonl(sub_event);
}

int ICEvent2::send(const CcltorIC *ic, const ICPeer *peer) const
{
	char buffer[SIZEOF_ICEVENT2];
	putEvent(buffer);
	putSubEvent(buffer);
	return send_msg(ic, peer, buffer, SIZEOF_ICEVENT2);
}

ICEvent2 *ICEvent2::create(int event, const char *msg, int len)
{
	if (len >= (SIZEOF_ICEVENT + sizeof(uint32_t)))
	{
		int sub_event = ntohl(*(uint32_t*)(msg + SIZEOF_ICEVENT));
		return new ICEvent2(event, sub_event);
	}
	return NULL;
}