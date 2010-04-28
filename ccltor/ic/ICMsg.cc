
#include "ICRegistration.h"
#include "ICEvent.h"
#include "ICMsgRaw.h"
#include "ICPeer.h"
#include "CcltorIC.h"
#include "ICMsg.h"

int ICMsg::send_msg(const CcltorIC *ic, const ICPeer *peer, char *buf, int len) const
{
	*(uint32_t*)buf = htonl(mclass);
	ic->send(peer, buf, len);
}

ICMsg *ICMsg::create(const char *msg, int len)
{
	if (len >= sizeof(uint32_t))
	{
		int msg_class = ntohl(*(uint32_t*)msg);
		switch (msg_class)
		{
		case ICMSGCLASS_RAW:			return new ICMsgRaw(msg, len);
		case ICMSGCLASS_EVENT:			return ICEvent::create(msg, len);
		case ICMSGCLASS_REGISTRATION:	return ICRegistration::create(msg, len);
		}
	}
	return NULL;
}
