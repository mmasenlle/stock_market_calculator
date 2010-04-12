
#include <arpa/inet.h>
#include "ICEvent2.h"
#include "ICEvent.h"

void ICEvent::putEvent(char *buf) const
{
	*(uint32_t*)(buf + SIZEOF_ICMSGCLASS) = htonl(event);
}

int ICEvent::send(const CcltorIC *ic, const ICPeer *peer) const
{
	char buffer[SIZEOF_ICEVENT];
	putEvent(buffer);
	return send_msg(ic, peer, buffer, SIZEOF_ICEVENT);
}

ICEvent *ICEvent::create(const char *msg, int len)
{
	if (len >= (SIZEOF_ICMSGCLASS + sizeof(uint32_t)))
	{
		int event = ntohl(*(uint32_t*)(msg + SIZEOF_ICMSGCLASS));
		if (event >= ICEVENT_EVENT2)
		{
			return ICEvent2::create(event, msg, len);
		}
		return new ICEvent(event);
	}
	return NULL;
}