
#include <arpa/inet.h>
#include "ICMsgRegister.h"

int ICMsgRegister::send(const CcltorIC *ic, const ICPeer *peer) const
{
	char buffer[SIZEOF_ICMSGCLASS + sizeof(uint32_t)];
	*(uint32_t*)(buffer + SIZEOF_ICMSGCLASS) = htonl(seconds);
	return send_msg(ic, peer, buffer, sizeof(buffer));
}

ICMsgRegister *ICMsgRegister::create(const char *msg, int len)
{
	if (len >= (SIZEOF_ICMSGCLASS + sizeof(uint32_t)))
	{
		return new ICMsgRegister(ntohl(*(uint32_t*)(msg + SIZEOF_ICMSGCLASS)));
	}
	return NULL;
}
