
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent2.h"
#include "ControlConfig.h"


ControlConfig::ControlConfig()
{
	event = sub_event = 0;
    ic_port = 17777;
}

bool ControlConfig::strto_event(const char *str, char **p)
{
	DLOG("ControlConfig::strto_event(%s)", str);
#define STR2EVENT(_s, _e) \
	if (!strncasecmp(str, _s, strlen(_s)) && (!str[strlen(_s)] || str[strlen(_s)] == ':')) { \
		*p = (char*)str + strlen(_s); \
		event = _e; \
		return true; \
	}
	STR2EVENT("run", ICEVENT_CONTROL_RUNNING);
	STR2EVENT("log", ICEVENT_CONTROL_LOGLEVEL);
	return false;
}

bool ControlConfig::strto_subevent(const char *str)
{
	DLOG("ControlConfig::strto_subevent(%s)", str);
#define STR2SEVENT(_s, _e) \
	if (strcasecmp(str, _s) == 0) { \
		sub_event = _e; \
		return true; \
	}
	STR2SEVENT("ping", ICEVENT2_CONTROL_RUNNING_PING);
	STR2SEVENT("stop", ICEVENT2_CONTROL_RUNNING_STOP);
	return false;
}

void ControlConfig::get_event(const char *ev_str)
{
	char *p;
	if (!strto_event(ev_str, &p))
		event = strtol(ev_str, &p, 10);
	if (p && *p++)
	{
		if (!strto_subevent(p))
			sub_event = strtol(p, NULL, 10);
	}
}

void ControlConfig::add_peer(const char *p_str)
{
	char *p = (char*)strchr(p_str, ':');
	if (p)
	{
		*p = 0;
		int port = atoi(++p);
		if (port > 0)
		{
			ICPeer peer;
			if (peer.set(p_str, port) == 0)
			{
				peers.push_back(peer);
			}
		}
	}
}

void ControlConfig::add_peers(const char *peers)
{
	char *tok, *p = strtok_r((char *)peers, ",", &tok);
	while(p) 
	{
		add_peer(p);
		p = strtok_r(NULL, ",", &tok);
	}
}

void ControlConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_control";
    XPathConfig xpconf(cfg_fname.c_str());

	init_post(xpconf, key.c_str(), argc, argv);

	if (xpconf.getValue((key + "/event").c_str(), &xp_value))
    	get_event(xp_value.c_str());
	int np = xpconf.getValue((key + "/peers/peer").c_str(), NULL);
	for(int i = 0; i < np; i++)
	{
		if (xpconf.getValue((key + "/peers/peer").c_str(), &xp_value, i))
			add_peer(xp_value.c_str());
	}

    FOR_OPT_ARG(argc, argv)
    {
    case 'e': get_event(arg); break;
    case 'p': add_peers(arg); break;
    }
    END_OPT;

    DLOG("ControlConfig::init() event = %d:%d", event, sub_event);
    for (int i = 0; i < peers.size(); i++)
    {
    	DLOG("ControlConfig::init() peer(%d) = 0x%08x:%d", i, peers[i].get_ip(), peers[i].get_port());
    }

	DLOG("ControlConfig::init() ic_port = %d", ic_port);
    DLOG("ControlConfig::init() log_level = %d", log_level);
    DLOG("ControlConfig::init() db_conninfo = '%s'", db_conninfo.c_str());
    DLOG("ControlConfig::init() cfg_fname = '%s'", cfg_fname.c_str());
    DLOG("ControlConfig::init() log_fname = '%s'", log_fname.c_str());
}

void ControlConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_control [options]\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -e <ev>[:<sev>] Event to send\n");
    fprintf(stdout, "  -p <host>:<port>[,...] Destination peer(s)\n");
}

