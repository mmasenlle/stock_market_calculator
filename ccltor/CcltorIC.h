#ifndef _CCLTORIC_H_
#define _CCLTORIC_H_

#include "CcltorICMsg.h"
#include "CcltorICPeer.h"

#define CCLTORIC_RECVERR_SYS (-1)
#define CCLTORIC_RECVERR_IGN (-2)
#define CCLTORIC_RECVERR_SHORT (-3)

enum
{
	CCLTORIC_REGISTER_FEEDEREVENTS,
	CCLTORIC_FEEDER_NOTIFY_NEWFEED,
};

class CcltorIC
{
	int sd;
	CcltorICPeer peer;

public:
	CcltorIC();
	~CcltorIC();
	int init(int port);
	int get_fd();
	int receive(CcltorICMsg *msg, CcltorICPeer *from) const;
	int send(const CcltorICPeer *to, CcltorICMsg *msg, int len) const;
};

#endif
