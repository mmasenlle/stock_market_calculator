
#include "utils.h"
#include "logger.h"
#include "SchedulerConfig.h"


SchedCmd::SchedCmd()
{
	days_off = time_start = interval = 0;
	argv = NULL;
	argbuf = NULL;
}

SchedCmd::~SchedCmd()
{
	delete [] argv;
	delete [] argbuf;
}

SchedulerConfig::SchedulerConfig()
{
    ic_port = 17600;
}

void SchedulerConfig::setCmd(const char *cmd, SchedCmd *scmd)
{
	DLOG("SchedulerConfig::setCmd(%s)", cmd);
	int bufsize = 0;
	std::vector<std::string> argv;
	std::string arg;
	for (; *cmd; cmd++)
	{
		if (isspace(*cmd))
		{
			if (arg.length())
			{
				argv.push_back(arg);
				bufsize += arg.size();
				arg.clear();
			}
		}
		else
		{
			arg.push_back(*cmd);
		}
	}
	if (arg.length())
	{
		argv.push_back(arg);
		bufsize += arg.size();
	}
	scmd->argv = new char*[argv.size() + 1];
	scmd->argbuf = new char[bufsize];
	char *p = scmd->argbuf;
	for (int i = 0; i < argv.size(); i++)
	{
		scmd->argv[i] = p;
		strcpy(p, argv[i].c_str());
		p += argv[i].size();
	}
	scmd->argv[argv.size()] = NULL;
}

void SchedulerConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_scheduler";
    XPathConfig xpconf(cfg_fname.c_str());

	init_post(xpconf, key.c_str(), argc, argv);

	int nc = xpconf.getValue((key + "/commands/command").c_str(), NULL);
	for(int i = 0; i < nc; i++)
	{
		char ncmd[16]; snprintf(ncmd, sizeof(ncmd), "[%d]", i);
		std::string ckey = key + "/commands/command"; ckey += ncmd;
		if (xpconf.getValue((ckey + "/cmd").c_str(), &xp_value, i))
		{
			SchedCmd schedCmd;
			setCmd(xp_value.c_str(), &schedCmd);
		    if (xpconf.getValue((key + "/days_off").c_str(), &xp_value))
		    {
		    	schedCmd.days_off = 0;
		        for (int i = 0; i < xp_value.length() && i < 7; i++)
		            if (xp_value[i] == '1') schedCmd.days_off |= (1 << i);
		    }
			if (xpconf.getValue((key + "/time_start").c_str(), &xp_value))
				schedCmd.time_start = utils::strtot(xp_value.c_str());
			if (xpconf.getValue((key + "/interval").c_str(), &xp_value))
				schedCmd.interval = atoi(xp_value.c_str());
			cmds.push_back(schedCmd);
		}
	}


    for (int i = 0; i < cmds.size(); i++)
    {
    	DLOG("SchedulerConfig::init() cmds[%d].days_off = %d", i, cmds[i].days_off);
    	DLOG("SchedulerConfig::init() cmds[%d].time_start = %d", i, cmds[i].time_start);
    	DLOG("SchedulerConfig::init() cmds[%d].interval = %d", i, cmds[i].interval);
    	for (int j = 0; cmds[i].argv[j]; j++)
        {
        	DLOG("SchedulerConfig::init() cmds[%d].argv[%d] = '%s'", i, j, cmds[i].argv[j]);
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
