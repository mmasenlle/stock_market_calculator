
#include "logger.h"
#include "ICEvent2.h"
#include "CcltorIC.h"
#include "ControlConfig.h"

int main(int argc, char *argv[])
{
	ControlConfig config;
	config.init(argc, argv);
	
	CcltorIC ic;
	if (ic.init(config.ic_port) == -1)
	{
		SELOG("ccltor_control() -> ic.init(%d)", config.ic_port);
		return -1;
	}

	if (config.event > ICEVENT_EVENT2)
	{
		ICEvent2 event(config.event, config.sub_event);
		for (int i = 0; i < config.peers.size(); i++)
		{
			if (event.send(&ic, &config.peers[i]) <= 0)
			{
				SELOG("ccltor_control() -> event.send(%d:%d, 0x%08x:%p)", config.event,
				    config.sub_event, config.peers[i].get_ip(), config.peers[i].get_port());
			}
			else
			{
				ILOG("ccltor_control() -> event %d:%d successfully sent to 0x%08x:%p", config.event,
				    config.sub_event, config.peers[i].get_ip(), config.peers[i].get_port());
			}
		}
	}
	else if (config.event > ICEVENT_EVENT)
	{
		ICEvent event(config.event);
		for (int i = 0; i < config.peers.size(); i++)
		{
			if (event.send(&ic, &config.peers[i]) <= 0)
			{
				SELOG("ccltor_control() -> event.send(%d, 0x%08x:%p)",
						config.event, config.peers[i].get_ip(), config.peers[i].get_port());
			}
			else
			{
				ILOG("ccltor_control() -> event %d successfully sent to 0x%08x:%p",
						config.event, config.peers[i].get_ip(), config.peers[i].get_port());
			}
		}
	}
}
