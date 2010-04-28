
#include <sys/types.h>
#include <sys/wait.h>
#include "utils.h"
#include "logger.h"
#include "ICEvent2.h"
#include "XmlReaderData.h"
#include "Feeder.h"

Feeder Feeder::feeder;

void Feeder::sigchld_handler(int s)
{
	int status, pid;
	if ((pid = wait(&status)) == -1)
	{
		SELOG("Feeder::sigchld_handler(%d) -> wait", s);
	}
	else
	{
		if (WIFEXITED(status))
		{
			char ret = WEXITSTATUS(status);
			DLOG("Feeder::sigchld_handler() -> %s(%d) ends with %d",
					(pid == Feeder::feeder.pid_wget) ? "wget" : "parser", pid, ret);
			if (ret != 0)
			{
				ELOG("Feeder::sigchld_handler() -> %s(%d) failed with %d",
				    (pid == Feeder::feeder.pid_wget) ? "wget" : Feeder::feeder.config.parser.c_str(), pid, ret);
			}
		}
		else
		{
			ELOG("Feeder::sigchld_handler() -> %s(%d) ends/hangs with a signal or other event",
			    (pid == Feeder::feeder.pid_wget) ? "wget" : Feeder::feeder.config.parser.c_str(), pid);
		}
	}
}

Feeder::Feeder() : newfeed(ICEVENT_FEEDER_NEWFEED), dbfeeder(&db)
{
	last_feed.tv_sec = last_feed.tv_usec = 0;
	pfds[0].fd = -1;
	pfds[0].events = POLLIN;
}

void Feeder::handleEvent(ICEvent *event, ICPeer *peer)
{
	ICEvent2 *ev2 = NULL;
	if (event->getEvent() > ICEVENT_EVENT2) ev2 = (ICEvent2 *)event;
	switch (event->getEvent())
	{
	case ICEVENT_CONTROL_RUNNING:
		ILOG("Feeder::handleEvent(RUNNING:%d)", ev2->getSubEvent());
		switch (ev2->getSubEvent())
		{
		case ICEVENT2_CONTROL_RUNNING_PING:
		{
			ICEvent2 ev(ICEVENT_CONTROL_RUNNING, ICEVENT2_CONTROL_RUNNING_ACK);
			ev.send(&ic, peer);
			break;
		}
		case ICEVENT2_CONTROL_RUNNING_STOP:
			exit(0);
		case ICEVENT2_CONTROL_RUNNING_RECONNECT:
			break;
		}
		break;
	case ICEVENT_CONTROL_LOGLEVEL:
		DLOG("Feeder::handleEvent(LOGLEVEL:%d)", ev2->getSubEvent());
		SET_LOGGER_LEVEL(ev2->getSubEvent());
		break;
	default:
		WLOG("Feeder::handleEvent() -> unhandled event %d", event->getEvent());
	}
}

void Feeder::handleIC()
{
	ICPeer peer;
	ICMsg *msg = NULL;
	if (ic.receive(&msg, &peer) < 0)
	{
		SELOG("Feeder::handleIC() -> ic.receive()");
	}
	else if(msg)
	{
		switch (msg->getClass())
		{
		case ICMSGCLASS_EVENT:
			handleEvent((ICEvent*)msg, &peer);
			break;
		case ICMSGCLASS_REGISTRATION:
			obs.add(&peer, (ICRegistration*)msg);
			break;
		default:
			WLOG("Feeder::handleIC() -> NOT expected msg class %d", msg->getClass());
		}
		delete msg;
	}
}


#define WGET_EXEC_ARGUMENTS(_url) "wget", "wget", "-q", "-O", "-", _url, NULL
#define PARSER_EXEC_ARGUMENTS(_parser) _parser, _parser, NULL

int Feeder::feed()
{
	DLOG("Feeder:feed(%s, %s)", config.url.c_str(), config.parser.c_str());
	int pd_wget[2];
	if (pipe(pd_wget) == -1)
	{
		SELOG("Feeder:feed() -> pipe(pd_wget)");
		return -1;
	}
	pid_wget = fork();
	if (pid_wget == -1)
	{
		SELOG("Feeder:feed() -> fork(wget)");
		close(pd_wget[0]); close(pd_wget[1]);
		return -1;
	}
	if (pid_wget == 0)
	{
		if (close(1) || dup(pd_wget[1]) != 1 || close(pd_wget[0]))
		{
			exit(-1);
		}
		execlp(WGET_EXEC_ARGUMENTS(config.url.c_str()));
		exit(-1);
	}
	close(pd_wget[1]);
	int pd_parser[2];
	if (pipe(pd_parser) == -1)
	{
		SELOG("Feeder:feed() -> pipe(pd_parser)");
		close(pd_wget[0]);
		return -1;
	}
	int pid_parser = fork();
	if (pid_parser == -1)
	{
		SELOG("Feeder:feed() -> fork(parser)");
		close(pd_wget[0]); close(pd_parser[0]); close(pd_parser[1]);
		wait(NULL);
		return -1;
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
		execlp(PARSER_EXEC_ARGUMENTS(config.parser.c_str()));
		exit(-1);
	}
	close(pd_wget[0]);
	close(pd_parser[1]);

	XmlReaderData xmlReader;
	for (;;)
	{
		char buffer[4096];
		int r = read(pd_parser[0], buffer, sizeof(buffer));
		if (r == 0) break;
		else if (r == -1 && errno == EINTR) continue;
		else if (r == -1)
		{
			SELOG("Feeder:feed() -> reading from parser");
			close(pd_parser[0]);
			return -1;
		}
		xmlReader.read(buffer, r);
	}
	close(pd_parser[0]);

	return xmlReader.get_count();
}

void Feeder::init()
{
	int r = snprintf(id_str, sizeof(id_str), "feeder:%d@", getpid());
	if (gethostname(id_str + r, sizeof(id_str) - r) < 0)
	{
		SELOG("Feeder::init() -> gethostname()");
	}
	if ((r = utils::nohup()) < 0)
	{
		SELOG("Feeder::init() -> utils::nohup(): %d", r);
	}
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		SELOG("Feeder::init() -> signal(SIGCHLD, sigchld_handler) == SIG_ERR");
	}
	if (ic.init(config.ic_port) == -1)
	{
		SELOG("Feeder::init() -> ic.init(%d)", config.ic_port);
	}
	if (db.connect(config.db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Feeder::init() -> db.connect(%s)", config.db_conninfo.c_str());
	}
	pfds[0].fd = ic.get_fd();
}

void Feeder::run()
{
    for (;;)
    {
        struct timeval now;
		if (gettimeofday(&now, NULL) == -1)
		{
			SELOG("Feeder::run() -> gettimeofday(now)");
		}
        struct tm tm;
		if (localtime_r(&now.tv_sec, &tm) == NULL)
		{
			SELOG("Feeder::run() -> localtime_r");
		}
        int wait_time, day_min = (tm.tm_hour * 60) + tm.tm_min;
        if ((config.days_off & (1 << tm.tm_wday)) || (config.time_stop < day_min))
        {
            wait_time = (24 * 3600) - ((day_min * 60) + tm.tm_sec);
        }
        else if (config.time_start > day_min)
        {
            wait_time = (config.time_start - day_min) * 60;
        }
        else
        {
            wait_time = last_feed.tv_sec + config.sdelay - now.tv_sec;
            if (wait_time <= 0) break;
        }
        DLOG("Feeder::run() -> wait_time %d", wait_time);
        if (poll(pfds, ARRAY_SIZE(pfds), wait_time * 1000) == -1 && errno != EINTR)
        {
            SELOG("Feeder::run() -> poll(%d, %d)", pfds[0].fd, wait_time * 1000);
        }
        else if (pfds[0].revents & POLLIN)
        {
        	handleIC();
        }
    }
	if (gettimeofday(&last_feed, NULL) == -1)
	{
		SELOG("Feeder::run() -> gettimeofday(last_feed)");
	}

	int r = feed();
	if (r > 0)
	{
		dbfeeder.insert_feed(id_str, r);
		obs.notify(&ic, &newfeed);
		ILOG("Feeder::run() -> %d values processed", r);
	}
	else
	{
		WLOG("Feeder::run() -> NO values processed at this feed (%d)", r);
	}
}
