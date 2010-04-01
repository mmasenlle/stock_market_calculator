#ifndef _FEEDERCONFIG_H_
#define _FEEDERCONFIG_H_

#include "CcltorConfig.h"

class FeederConfig : public CcltorConfig
{
public:
    int sdelay;
    int days_off;
    int time_start, time_end;
    int register_port;
    std::string url;

    static void init(int argc, char *argv[]);
    static FeederConfig feederConfig;
};

#endif

