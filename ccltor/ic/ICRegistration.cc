
#include <arpa/inet.h>
#include "ICRegistration.h"

int ICRegistration::send(const CcltorIC *ic, const ICPeer *peer) const
{
	char buffer[SIZEOF_ICMSGCLASS + sizeof(uint32_t)];
	*(uint32_t*)(buffer + SIZEOF_ICMSGCLASS) = htonl(seconds);
	return send_msg(ic, peer, buffer, sizeof(buffer));
}

ICRegistration *ICRegistration::create(const char *msg, int len)
{
	if (len >= (SIZEOF_ICMSGCLASS + sizeof(uint32_t)))
	{
		return new ICRegistration(ntohl(*(uint32_t*)(msg + SIZEOF_ICMSGCLASS)));
	}
	return NULL;
}
