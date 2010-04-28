#ifndef _CCLTORIC_H_
#define _CCLTORIC_H_

#include "ICPeer.h"
#include "ICMsg.h"

class CcltorIC
{
	int sd;
	ICPeer peer;
	
	friend class ICMsg;
	int send(const ICPeer *peer, const char *buffer, int len) const;
public:
	CcltorIC();
	~CcltorIC();
	int init(int port);
	int get_fd();
	int receive(ICMsg **msg, ICPeer *from = NULL) const;
};

#endif
