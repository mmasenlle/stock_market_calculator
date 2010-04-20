
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "logger.h"
#include "ICPeer.h"

ICPeer::ICPeer()
{
	clear();
}

bool ICPeer::operator<(const ICPeer &peer) const
{
	return (memcmp(this, &peer, sizeof(*this)) < 0);
}

void ICPeer::clear()
{
	memset(&saddr_in, 0, sizeof(saddr_in));
	addrlen = sizeof(saddr_in);
}

int ICPeer::set(const char *host, int port)
{
	int ret = 0;
	clear();
	saddr_in.sin_family = AF_INET;
	saddr_in.sin_port = htons(port);
	if(!inet_aton(host, &saddr_in.sin_addr))
	{
		struct addrinfo *addr_info = NULL;
		struct addrinfo hints = { 0 };
		hints.ai_family = PF_INET;
		hints.ai_socktype = SOCK_STREAM;
		if((ret = getaddrinfo(host, NULL, &hints, &addr_info)) == 0)
		{
			saddr_in.sin_addr = ((struct sockaddr_in *)(addr_info->ai_addr))->sin_addr;
			freeaddrinfo(addr_info);
		}
		else
		{
			ELOG("ICPeer::set(%s, %d) -> error resolving %d", host, port, ret);
		}
	}
	return ret;
}

unsigned long ICPeer::get_ip() const
{
	return ntohl(saddr_in.sin_addr.s_addr);
}

int ICPeer::get_port() const
{
	return ntohs(saddr_in.sin_port);
}
