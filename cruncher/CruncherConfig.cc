
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "logger.h"
#include "CruncherConfig.h"


CruncherConfig::CruncherConfig()
{
	shots = 0;
	force_until = 0;
	plugins_path = ".";
    ic_port = 17300;
}

void CruncherConfig::add_plugins(const char *ps)
{
	char *tok, *p = strtok_r((char *)ps, ",", &tok);
	while(p) 
	{
		plugins.push_back(p);
		p = strtok_r(NULL, ",", &tok);
	}
}

void CruncherConfig::add_feeder(const char *p_str)
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
				feeders.push_back(peer);
			}
		}
	}
}

void CruncherConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_cruncher";
    XPathConfig xpconf(cfg_fname.c_str());
    
    init_post(xpconf, key.c_str(), argc, argv);

	if (xpconf.getValue((key + "/shots").c_str(), &xp_value))
		shots = atoi(xp_value.c_str());
	if (xpconf.getValue((key + "/force_until").c_str(), &xp_value))
        force_until = atoi(xp_value.c_str());
    xpconf.getValue((key + "/plugins_path").c_str(), &plugins_path);
	int np = xpconf.getValue((key + "/plugins/plugin").c_str(), NULL);
	for(int i = 0; i < np; i++)
	{
		if (xpconf.getValue((key + "/plugins/plugin").c_str(), &xp_value, i))
			plugins.push_back(xp_value);
	}
	np = xpconf.getValue((key + "/feeders/feeder").c_str(), NULL);
	for(int i = 0; i < np; i++)
	{
		if (xpconf.getValue((key + "/feeders/feeder").c_str(), &xp_value, i))
			add_feeder(xp_value.c_str());
	}

	FOR_OPT(argc, argv)
    {
    case 's': shots = 1; break;
	}
	END_OPT;
    FOR_OPT_ARG(argc, argv)
    {
    case 'P': plugins_path = arg; break;
    case 'p': add_plugins(arg); break;
    case 's': shots = atoi(arg); break;
	case 'u': force_until = atoi(arg); break;
    }
    END_OPT;

	DLOG("CruncherConfig::init() force_until = %d", force_until);
    DLOG("CruncherConfig::init() plugins_path = '%s'", plugins_path.c_str());
    for (int i = 0; i < plugins.size(); i++)
    {
    	DLOG("CruncherConfig::init() plugins(%d) = '%s'", i, plugins[i].c_str());
    }
    for (int i = 0; i < feeders.size(); i++)
    {
    	DLOG("CruncherConfig::init() feeder(%d) = 0x%08x:%d", i, feeders[i].get_ip(), feeders[i].get_port());
    }

	DLOG("CruncherConfig::init() ic_port = %d", ic_port);
    DLOG("CruncherConfig::init() log_level = %d", log_level);
    DLOG("CruncherConfig::init() db_conninfo = '%s'", db_conninfo.c_str());
    DLOG("CruncherConfig::init() cfg_fname = '%s'", cfg_fname.c_str());
    DLOG("CruncherConfig::init() log_fname = '%s'", log_fname.c_str());
}

void CruncherConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_control [options]\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -P <path>       Plugin files path\n");
    fprintf(stdout, "  -p <plugin>[, ] Plugin(s) to load\n");
	fprintf(stdout, "  -u <day>        Force calculations until this day\n");
}

