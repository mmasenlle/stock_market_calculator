#ifndef _FEEDERCONFIG_H_
#define _FEEDERCONFIG_H_

#include "CcltorConfig.h"

class FeederConfig : public CcltorConfig
{
    void print_help();

public:
    int sdelay;
    int days_off;
    int time_start, time_stop;
    std::string url;
    std::string parser;

    FeederConfig();
    void init(int argc, char *argv[]);
};

#endif

