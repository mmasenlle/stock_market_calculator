#ifndef _SCHEDULERCONFIG_H_
#define _SCHEDULERCONFIG_H_

#include <vector>
#include "CcltorConfig.h"

struct SchedCmd
{
	int days_off;
	int time_start;
	int interval;

	char **argv;
	char *argbuf;
	SchedCmd();
	~SchedCmd();
};

class SchedulerConfig : public CcltorConfig
{
    void print_help();
    
    void setCmd(const char *cmd, SchedCmd *scmd);

public:
	std::vector<SchedCmd> cmds;

    SchedulerConfig();
    void init(int argc, char *argv[]);
};

#endif

