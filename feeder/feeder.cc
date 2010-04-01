
#include <sys/time.h>
#include <time.h>
#include <poll.h>
#include "logger.h"
#include "FeederConfig.h"

#define SECONDS_IN_A_DAY (24 * 3600)

int main(int argc, char *argv[])
{
    FeederConfig::init(argc, argv);

    struct timeval fetch_timestamp;
    for (;;)
    {
        for (;;)
        {
            struct timeval now; gettimeofday(&now, NULL);
            struct tm tm; localtime_r(&now.tv_sec, &tm);
            int wait_time, day_min = (tm.tm_hour * 60) + tm.tm_min;
            if ((FeederConfig::feederConfig.days_off & (1 << tm.tm_wday)) ||
                (FeederConfig::feederConfig.time_end > day_min))
            {
                wait_time = SECONDS_IN_A_DAY - (now.tv_sec % SECONDS_IN_A_DAY);
            }
            else if (FeederConfig::feederConfig.time_start < day_min)
            {
                wait_time = (day_min - FeederConfig::feederConfig.time_start) * 60;
            }
            else
            {
                wait_time = now.tv_sec -
                    fetch_timestamp.tv_sec - FeederConfig::feederConfig.sdelay;
                if (wait_time <= 0) break;
            }
            DLOG("main() -> wait_time %d", wait_time);
            if (poll(NULL, 0, wait_time * 1000) == -1)
            {
                SELOG("main() -> poll(%d, %d, %d)", 0, 0, wait_time * 1000);
            }
        }
        // fetch page
        // parse page
        // store page
        // notify
    }
}

            
