#ifndef _CRUNCHERTEST_H_
#define _CRUNCHERTEST_H_

#include "ICruncher.h"

class CruncherTest : public ICruncher
{
	ICruncherManager *manager;

public:
	int init(ICruncherManager *icm, Logger *logger);
	int run();
	int msg(ICMsg *msg);
};

#endif
