
#include "utils.h"
#include "logger.h"
#include "SchedulerConfig.h"


SchedCmd::SchedCmd()
{
	days_off = time_start = interval = 0;
	argv = NULL;
}

SchedCmd::~SchedCmd()
{
	delete [] argv;
}

void SchedCmd::set_cmd(const char *cmd)
{
	DLOG("SchedCmd::set_cmd(%s)", cmd);
	std::string arg;
	for (; *cmd; cmd++)
	{
		if (isspace(*cmd))
		{
			if (arg.length())
			{
				varg.push_back(arg);
				arg.clear();
			}
		}
		else
		{
			arg.push_back(*cmd);
		}
	}
	if (arg.length())
		varg.push_back(arg);

	argv = new const char*[varg.size() + 1];
	argv[varg.size()] = NULL;
	for (int i = 0; i < varg.size(); i++)
		argv[i] = varg[i].c_str();
}

void SchedCmd::set_do(const char *sdo)
{
	for (int j = 0; sdo[j] && j < 7; j++)
		if (sdo[j] == '1')
			days_off |= (1 << j);
}

void SchedCmd::set_ts(const char *sts)
{
	int ts = utils::strtot(sts);
	time_start = ts % 100;
	ts /= 100;
	time_start += (ts % 100) * 60;
	ts /= 100;
	time_start += ts * 60 * 60;
}

SchedulerConfig::SchedulerConfig()
{
    ic_port = 17600;
}

void SchedulerConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_scheduler";
    XPathConfig xpconf(cfg_fname.c_str());

	init_post(xpconf, key.c_str(), argc, argv);

	int nc = xpconf.getValue((key + "/commands/command").c_str(), NULL);
	for(int i = 1; i <= nc; i++)
	{
		char ncmd[16]; snprintf(ncmd, sizeof(ncmd), "[%d]", i);
		std::string ckey = key + "/commands/command"; ckey += ncmd;
		if (xpconf.getValue((ckey + "/cmd").c_str(), &xp_value))
		{
			SchedCmd *schedCmd = new SchedCmd;
			schedCmd->set_cmd(xp_value.c_str());
		    if (xpconf.getValue((ckey + "/days_off").c_str(), &xp_value))
				schedCmd->set_do(xp_value.c_str());
			if (xpconf.getValue((ckey + "/time_start").c_str(), &xp_value))
				schedCmd->set_ts(xp_value.c_str());
			if (xpconf.getValue((ckey + "/interval").c_str(), &xp_value))
				schedCmd->interval = atoi(xp_value.c_str());
			cmds.push_back(schedCmd);
		}
	}


    for (int i = 0; i < cmds.size(); i++)
    {
    	DLOG("SchedulerConfig::init() cmds[%d].days_off = %d", i, cmds[i]->days_off);
    	DLOG("SchedulerConfig::init() cmds[%d].time_start = %d", i, cmds[i]->time_start);
    	DLOG("SchedulerConfig::init() cmds[%d].interval = %d", i, cmds[i]->interval);
    	for (int j = 0; cmds[i]->argv[j]; j++)
        {
        	DLOG("SchedulerConfig::init() cmds[%d].argv[%d] = '%s'", i, j, cmds[i]->argv[j]);
        }
    }    
	DLOG("SchedulerConfig::init() ic_port = %d", ic_port);
    DLOG("SchedulerConfig::init() log_level = %d", log_level);
    DLOG("SchedulerConfig::init() db_conninfo = '%s'", db_conninfo.c_str());
    DLOG("SchedulerConfig::init() cfg_fname = '%s'", cfg_fname.c_str());
    DLOG("SchedulerConfig::init() log_fname = '%s'", log_fname.c_str());
}

void SchedulerConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_scheduler [options]\n");
    fprintf(stdout, "Options:\n");
}
