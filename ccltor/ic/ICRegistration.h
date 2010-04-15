#ifndef _ICREGISTRATION_H_
#define _ICREGISTRATION_H_

#include "ICMsg.h"

class ICRegistration : public ICMsg
{
	int seconds;
public:
	ICRegistration(int sec) : ICMsg(ICMSGCLASS_REGISTRATION), seconds(sec) {};
	int getSeconds() const { return seconds; };
	int send(const CcltorIC *ic, const ICPeer *peer) const;
	static ICRegistration *create(const char *msg, int len);
};

#endif /*_ICREGISTRATION_H_*/
