#ifndef _SCHEDULERCONFIG_H_
#define _SCHEDULERCONFIG_H_

#include <vector>
#include "CcltorConfig.h"

struct SchedCmd
{
	int days_off;
	int time_start;
	int interval;
	std::vector<std::string> varg;
	const char **argv;

	SchedCmd();
	~SchedCmd();
	void set_cmd(const char *cmd);
	void set_do(const char *sdo);
	void set_ts(const char *sts);
};

class SchedulerConfig : public CcltorConfig
{
    void print_help();

public:
	std::vector<SchedCmd*> cmds;

    SchedulerConfig();
    void init(int argc, char *argv[]);
};

#endif

