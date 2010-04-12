#ifndef ICMSGREGISTER_H_
#define ICMSGREGISTER_H_

#include "ICMsg.h"

class ICMsgRegister : public ICMsg
{
	int seconds;
public:
	ICMsgRegister(int sec) : ICMsg(ICMSGCLASS_REGISTER), seconds(sec) {};
	int getSeconds() { return seconds; };
	int send(const CcltorIC *ic, const ICPeer *peer) const;
	static ICMsgRegister *create(const char *msg, int len);
};

#endif /*ICMSGREGISTER_H_*/
