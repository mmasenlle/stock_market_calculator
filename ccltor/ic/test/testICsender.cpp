
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "ICMsgRaw.h"
#include "ICEvent2.h"
#include "ICMsgRegister.h"
#include "CcltorIC.h"

int main(int argc, char *argv[])
{
	INIT_DEFAULT_LOGGER;
	
	int lport = 17001;
	int rport = 17000;
	if (argc > 1) lport = atoi(argv[1]);
	if (argc > 2) rport = atoi(argv[2]);
	
	ILOG("testICsender-> using lport %d", lport);
{	CcltorIC ic;
	int r = ic.init(lport);
	DLOG("ic.init(%d) = %d - press enter to continue", lport, r);
	if (r < 0)
	{
		SELOG("ic.init(%d)", lport);
	}
	getchar();
	
	ILOG("testICsender-> using rport localhost:%d", rport);
	ICPeer peer;
	r = peer.set("localhost", rport);
	DLOG("peer.set(localhost, %d) = %d - press enter to continue", rport, r);
	getchar();
	
	ILOG("testICsender-> sending raw msg ...");
	ICMsgRaw raw;
	raw.size = 5;
	raw.data = "hola";
	r = raw.send(&ic, &peer);
	DLOG("raw.send() = %d - press enter to continue", r);
	if (r < 0)
	{
		SELOG("raw.send()");
	}
	getchar();
	
	ILOG("testICsender-> sending event msg ...");
	ICEvent event(1);
	r = event.send(&ic, &peer);
	DLOG("event.send() = %d - press enter to continue", r);
	if (r < 0)
	{
		SELOG("event.send()");
	}
	getchar();
	
	ILOG("testICsender-> sending event msg ...");
	ICEvent2 event2(5, 2);
	r = event2.send(&ic, &peer);
	DLOG("event2.send() = %d - press enter to continue", r);
	if (r < 0)
	{
		SELOG("event2.send()");
	}
	getchar();
	
	ILOG("testICsender-> sending event msg ...");
	ICMsgRegister reg(100);
	r = reg.send(&ic, &peer);
	DLOG("reg.send() = %d - press enter to continue", r);
	if (r < 0)
	{
		SELOG("reg.send()");
	}
}	getchar();

	DLOG("testICsender-> we are done");
}
