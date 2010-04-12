#ifndef _ICPEER_H_
#define _ICPEER_H_

#include <sys/types.h>
#include <netinet/in.h>

class ICPeer
{
	friend class CcltorIC;
	struct sockaddr_in saddr_in;
	socklen_t addrlen;
public:
	ICPeer();
	bool operator<(const ICPeer &peer) const;
	void clear();
	int set(const char *host, int port);
};

#endif
