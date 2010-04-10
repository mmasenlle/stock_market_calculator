#ifndef _CCLTORICMSG_H_
#define _CCLTORICMSG_H_

#include <arpa/inet.h>

#define CCLTORICMSG_MAXDATALEN 1532

class CcltorICMsg
{
	friend class CcltorIC;
	inline char *ptr() { return (char*)this; };
	inline void *to_net() { head = htonl(head); return this; };
	inline void from_net() { head = ntohl(head); };
public:
	unsigned long head;
	char data[CCLTORICMSG_MAXDATALEN];	
};

#endif
