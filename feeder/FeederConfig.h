#ifndef _FEEDERCONFIG_H_
#define _FEEDERCONFIG_H_

#include "CcltorConfig.h"

class FeederConfig : public CcltorConfig
{
    FeederConfig();
    void print_help();

public:
    int sdelay;
    int days_off;
    int time_start, time_stop;
    int events_port;
    std::string url;
    std::string parser;

    static void init(int argc, char *argv[]);
    static FeederConfig feederConfig;
};

#endif

