
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <poll.h>
#include "logger.h"
#include "FeederConfig.h"
#include "XmlParser.h"

#define WGET_EXEC_ARGUMENTS(_url) "wget", "wget", "-q", "-O", "-", _url, NULL
#define PARSER_EXEC_ARGUMENTS(_parser) _parser, _parser, NULL

static int pid_wget;
static void sigchld_handler(int s)
{
	int status, pid;
	if ((pid = wait(&status)) == -1)
	{
		SELOG("feeder:feed() -> wait");
	}
	else
	{
		if (WIFEXITED(status))
		{
			char ret = WEXITSTATUS(status);
			DLOG("feeder:feed() -> %s(%d) ends with %d", (pid == pid_wget) ? "wget" : "parser", pid, ret);
			if (ret != 0)
			{
				ELOG("feeder:feed() -> %s(%d) failed with %d",
				    (pid == pid_wget) ? "wget" : FeederConfig::feederConfig.parser.c_str(), pid, ret);
			}
		}
		else
		{
			ELOG("feeder:feed() -> %s(%d) ends/hangs with a signal or other event",
			    (pid == pid_wget) ? "wget" : FeederConfig::feederConfig.parser.c_str(), pid);
		}
	}
}

static void feed(struct timeval *fetch_timestamp)
{
	DLOG("feeder:feed(%s, %s)", FeederConfig::feederConfig.url.c_str(), FeederConfig::feederConfig.parser.c_str());
	int pd_wget[2];
	if (pipe(pd_wget) == -1)
	{
		SELOG("feeder:feed() -> pipe(pd_wget)");
		return;
	}
	pid_wget = fork();
	if (pid_wget == -1)
	{
		SELOG("feeder:feed() -> fork(wget)");
		close(pd_wget[0]); close(pd_wget[1]);
		return;
	}
	if (pid_wget == 0)
	{
		if (close(1) || dup(pd_wget[1]) != 1 || close(pd_wget[0]))
        {
			exit(-1);
		}
        execlp(WGET_EXEC_ARGUMENTS(FeederConfig::feederConfig.url.c_str()));
        exit(-1);
	}
	close(pd_wget[1]);
	int pd_parser[2];
	if (pipe(pd_parser) == -1)
	{
		SELOG("feeder:feed() -> pipe(pd_parser)");
		close(pd_wget[0]);
		return;
	}
	int pid_parser = fork();
	if (pid_parser == -1)
	{
		SELOG("feeder:feed() -> fork(parser)");
		close(pd_wget[0]); close(pd_parser[0]); close(pd_parser[1]);
		wait(NULL);
		return;
	}
	if (pid_parser == 0)
	{
		if (close(0) || dup(pd_wget[0]) != 0)
        {
			exit(-1);
		}
		if (close(1) || dup(pd_parser[1]) != 1 || close(pd_parser[0]))
        {
			exit(-1);
		}
        execlp(PARSER_EXEC_ARGUMENTS(FeederConfig::feederConfig.parser.c_str()));
        exit(-1);
	}
	close(pd_wget[0]);
	close(pd_parser[1]);

	XmlParser xmlParser;
	for (;;)
	{
		char buffer[4096];
		int r = read(pd_parser[0], buffer, sizeof(buffer));
		if (r == 0) break;
		else if (r == -1 && errno == EINTR) continue;
		else if (r == -1)
		{
			SELOG("feeder:feed() -> reading from parser");
			close(pd_parser[0]);
			return;
		}
		xmlParser.parse(buffer, r);
	}
	close(pd_parser[0]);
	if (gettimeofday(fetch_timestamp, NULL) == -1)
	{
		SELOG("feeder:feed() -> gettimeofday");
	}
	xmlParser.store(fetch_timestamp);
}

int main(int argc, char *argv[])
{
    FeederConfig::init(argc, argv);
	signal(SIGCHLD, sigchld_handler);

    struct timeval fetch_timestamp = { 0 };
    for (;;)
    {
        for (;;)
        {
            struct timeval now;
			if (gettimeofday(&now, NULL) == -1)
			{
				SELOG("feeder:main() -> gettimeofday");
			}
            struct tm tm;
			if (localtime_r(&now.tv_sec, &tm) == NULL)
			{
				SELOG("feeder:main() -> localtime_r");
			}
            int wait_time, day_min = (tm.tm_hour * 60) + tm.tm_min;
            if ((FeederConfig::feederConfig.days_off & (1 << tm.tm_wday)) ||
                (FeederConfig::feederConfig.time_stop < day_min))
            {
                wait_time = (24 * 3600) - ((day_min * 60) + tm.tm_sec);
            }
            else if (FeederConfig::feederConfig.time_start > day_min)
            {
                wait_time = (FeederConfig::feederConfig.time_start - day_min) * 60;
            }
            else
            {
                wait_time = fetch_timestamp.tv_sec +
					FeederConfig::feederConfig.sdelay - now.tv_sec;
                if (wait_time <= 0) break;
            }
            DLOG("feeder:main() -> wait_time %d", wait_time);
            if (poll(NULL, 0, wait_time * 1000) == -1 && errno != EINTR)
            {
                SELOG("feeder:main() -> poll(%d, %d, %d)", 0, 0, wait_time * 1000);
            }
        }
        feed(&fetch_timestamp);
		
        //TODO: notify ?
    }
}

            
