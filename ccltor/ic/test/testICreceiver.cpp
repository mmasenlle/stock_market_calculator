
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
	
	int port = 17000;
	if (argc > 1) port = atoi(argv[1]);
	
	ILOG("testICreceiver-> using port %d", port);
	CcltorIC ic;
	int r = ic.init(port);
	DLOG("ic.init(%d) = %d - press enter to receive", port, r);
	if (r < 0)
	{
		SELOG("ic.init(%d)", port);
	}

	while (getchar() != 'q')
	{
		ICPeer peer;
		ICMsg *msg = NULL;
		r = ic.receive(&msg, &peer);
		DLOG("ic.receive() = %d, @msg = %p, from: %08x:%d", r, msg, peer.get_ip(), peer.get_port());
		if (r <= 0)
		{
			SELOG("ic.receive()");
		}
		else if (msg)
		{
			ILOG("testICreceiver-> msg_class: %d", msg->getClass());
			switch (msg->getClass())
			{
			case ICMSGCLASS_RAW:
			{
				ICMsgRaw *raw = (ICMsgRaw *)msg;
				ILOG("testICreceiver-> ICMsgRaw: size %d, @data %p, @mem %p", raw->size, raw->data, raw->mem);
				break;
			}
			case ICMSGCLASS_EVENT:
			{
				ICEvent *event = (ICEvent *)msg;
				ILOG("testICreceiver-> ICEvent: event %d", event->getEvent());
				if (event->getEvent() >= ICEVENT_EVENT2)
				{
					ICEvent2 *event2 = (ICEvent2 *)msg;
					ILOG("testICreceiver-> ICEvent2: sub_event %d", event2->getSubEvent());
				}
				break;
			}
			case ICMSGCLASS_REGISTER:
			{
				ICMsgRegister *reg = (ICMsgRegister *)msg;
				ILOG("testICreceiver-> ICMsgRegister: seconds %d", reg->getSeconds());
				break;
			}
			default:
				WLOG("testICreceiver-> unknown msg class: %d", msg->getClass());
			}
		}
		if (msg) delete msg;
	}

	DLOG("testICreceiver-> ciao");
}
