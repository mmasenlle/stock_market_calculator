#ifndef _CCLTORICPEER_H_
#define _CCLTORICPEER_H_

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

class CcltorICPeer
{
	friend class CcltorIC;
	struct sockaddr_in saddr_in;
	socklen_t addrlen;
public:
	bool operator<(const CcltorICPeer &peer) const
	{ return (memcmp(this, &peer, sizeof(*this)) < 0); }
	void clear() { memset(&saddr_in, 0, sizeof(saddr_in)); addrlen = sizeof(saddr_in); };
	CcltorICPeer() { clear(); };
};

#define SADDR(_peer) ((struct sockaddr *)&(_peer).saddr_in)

#endif
