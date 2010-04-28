
#include "logger.h"
#include "CruncherTest.h"


extern "C" ICruncher * CRUNCHER_GETINSTANCE()
{
	return new CruncherTest;
}

int CruncherTest::init(ICruncherManager *icm)
{
	DLOG("CruncherTest::init(%p) -> pid: %d, this: %p, ~stack: %p", icm, getpid(), this, &icm);
	manager = icm;
	return 0;
}

int CruncherTest::run()
{
	int var = 100;
	DLOG("CruncherTest::run() -> pid: %d, this: %p, ~stack: %p IN", getpid(), this, &var);
	manager->observe(5);
	sleep(var);
	manager->observe(4);
	DLOG("CruncherTest::run() -> pid: %d, this: %p, ~stack: %p RETURNING", getpid(), this, &var);
	return var;
}

int CruncherTest::msg(ICMsg *msg)
{
	DLOG("CruncherTest::msg(%p) -> pid: %d", msg, getpid());
	return 0;
}
