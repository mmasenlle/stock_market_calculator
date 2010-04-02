
#include "FeederConfig.h"

FeederConfig FeederConfig::feederConfig;

FeederConfig::FeederConfig()
{
    sdelay = 5 * 60;
    days_off = 0;
    time_start = 0;
    time_end = 24 * 60;
    events_port = 20800;
}

void FeederConfig::init(int argc, char *argv[])
{

}

