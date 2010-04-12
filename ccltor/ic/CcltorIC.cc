
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "CcltorIC.h"

CcltorIC::CcltorIC() : sd(-1) {}

CcltorIC::~CcltorIC()
{
	if (sd != -1)
	{
		close(sd);
		sd = -1;
	}
}

#define SADDR(_peer) ((struct sockaddr *)&(_peer).saddr_in)

int CcltorIC::init(int port)
{
	if (sd != -1) close(sd);
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		return -1;
	peer.clear();
	peer.saddr_in.sin_family = AF_INET;
	peer.saddr_in.sin_port = htons(port);
	return bind(sd, SADDR(peer), peer.addrlen);
}

int CcltorIC::get_fd()
{
	if (sd != -1)
	{
		int flags = fcntl(sd, F_GETFD);
		if ((flags == -1) || (fcntl(sd, F_SETFD, flags | O_NONBLOCK) == -1))
		{
			return -1;
		}
	}
	return sd;
}

int CcltorIC::receive(ICMsg **msg, ICPeer *from) const
{
	int ret = -1;
	char buffer[2048];
	if (from)
	{
		from->clear();
		ret = recvfrom(sd, buffer, sizeof(buffer), 0, SADDR(*from), &from->addrlen);
	}
	else
	{
		ret = recv(sd, buffer, sizeof(buffer), 0);
	}
	if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return 0;

	if (ret > 0)
	{
		*msg = ICMsg::create(buffer, ret);
	}

	return ret;
}

int CcltorIC::send(const ICPeer *to, const char *buffer, int len) const
{
	return sendto(sd, buffer, len, 0, SADDR(*to), to->addrlen);
}
