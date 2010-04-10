
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

int CcltorIC::receive(CcltorICMsg *msg, CcltorICPeer *from) const
{
	from->clear();
	int r = recvfrom(sd, msg->ptr(), sizeof(*msg), 0, SADDR(*from), &from->addrlen);
	if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return CCLTORIC_RECVERR_IGN;
	if (r < 0)
		return CCLTORIC_RECVERR_SYS;
	if (r < sizeof(msg->head))
		return CCLTORIC_RECVERR_SHORT;

	msg->from_net();
	return (r - sizeof(msg->head));
}

int CcltorIC::send(const CcltorICPeer *to, CcltorICMsg *msg, int len) const
{
	return
		(sendto(sd, msg->to_net(), len + sizeof(msg->head), 0, SADDR(*to), to->addrlen) - sizeof(msg->head));
}
