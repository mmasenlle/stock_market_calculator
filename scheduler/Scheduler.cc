#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include "logger.h"
#include "utils.h"
#include "ICEvent2.h"
#include "Scheduler.h"

Scheduler::Scheduler()
{
	next = -1;
	delay = 0;
	next_time = 0;
}

void Scheduler::update(time_t tt)
{
	next = -1;
    struct tm tm; localtime_r(&tt, &tm);
    int daysec = (((tm.tm_hour * 60) + tm.tm_min) * 60) + tm.tm_sec;
	for (int i = 0; i < config.cmds.size(); i++)
	{
		int rem = delay;
		if (last_stamps[i] && config.cmds[i].interval)
		{
			rem = tt + config.cmds[i].interval - last_stamps[i];
			for (int j = 0; j < 7 && (config.cmds[i].days_off & (1 << ((tm.tm_wday + j) % 7))); j++)
				rem += (24 * 60 * 60); //FIXME: jump a whole day or start in time start ?
		}
		else
		{
			rem = config.cmds[i].time_start - daysec;
			for (int j = 0; j < 7 && (config.cmds[i].days_off & (1 << ((tm.tm_wday + j) % 7))); j++)
				rem += (24 * 60 * 60);
			if (rem < 0 && (!last_stamps[i] || (last_stamps[i] + 100 > tt)))
				rem += (24 * 60 * 60);
		}
		if (next < 0 || (rem < delay))
		{
			next = i;
			delay = rem;
		}
	}
	next_time = delay > 0 ? tt + delay : tt;
}

void Scheduler::exec()
{
	ILOG("Scheduler::exec(%d) -> '%s'", next, config.cmds[next].argv[0]);
	int pid = fork();
	if (pid == -1)
	{
		SELOG("Scheduler::exec() -> fork()");
	}
	else if (pid == 0)
	{
		execvp(config.cmds[next].argv[0], config.cmds[next].argv);
		SELOG("Scheduler::exec(%d) -> '%s'", next, config.cmds[next].argv[0]);
		exit(-1);
	}
}

void Scheduler::init()
{
	if (utils::nohup() < 0)
	{
		SELOG("Scheduler::init() -> utils::nohup()");
	}
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		SELOG("Scheduler::init() -> signal(SIGCHLD, sigchld_handler) == SIG_ERR");
	}
	if (ic.init(config.ic_port) == -1)
	{
		SELOG("Scheduler::init() -> ic.init(%d)", config.ic_port);
	}
	last_stamps.resize(config.cmds.size(), 0);
}

void Scheduler::handle_msg(ICMsg *msg, ICPeer *from)
{
	DLOG("Scheduler::handle_msg(%d)", msg->getClass());
	switch (msg->getClass())
	{
	case ICMSGCLASS_EVENT:
	{
		ICEvent *ev = (ICEvent *)msg;
		ICEvent2 *ev2 = (ev->getEvent() > ICEVENT_EVENT2) ? (ICEvent2 *)ev : NULL;
		switch (ev->getEvent())
		{
		case ICEVENT_CONTROL_RUNNING:
			ILOG("Scheduler::handle_msg(RUNNING:%d)", ev2->getSubEvent());
			switch (ev2->getSubEvent())
			{
			case ICEVENT2_CONTROL_RUNNING_PING:
			{
				ICEvent2 ack(ICEVENT_CONTROL_RUNNING, ICEVENT2_CONTROL_RUNNING_ACK);
				ack.send(&ic, from);
				break;
			}
			case ICEVENT2_CONTROL_RUNNING_STOP:
				exit(0);
			}
			break;
		case ICEVENT_CONTROL_LOGLEVEL:
			DLOG("Scheduler::handle_msg(LOGLEVEL:%d)", ev2->getSubEvent());
			SET_LOGGER_LEVEL(ev2->getSubEvent());
			break;
		default:
			WLOG("Scheduler::handle_msg() -> unhandled event %d", ev->getEvent());
		}
		break;
	}
	default:
		WLOG("Scheduler::handle_msg() -> NOT expected msg class %d", msg->getClass());
	}
}

void Scheduler::run()
{
	struct pollfd pfds[1];
	pfds[0].fd = ic.get_fd();
	pfds[0].events = POLLIN;

	update(time(NULL));

	for (;;)
	{
		int wait_time = -1;
		time_t tt = time(NULL);
		while (next >= 0)
		{
			wait_time = next_time - tt;
			if (wait_time > 0 && wait_time <= delay)
				break;
			exec();
			last_stamps[next] = tt;
			update(tt);
		}
		DLOG("Scheduler::run() -> wait_time %d", wait_time);
        if (poll(pfds, ARRAY_SIZE(pfds), wait_time * 1000) == -1 && errno != EINTR)
        {
            SELOG("Scheduler::run() -> poll(%d, %d)", pfds[0].fd, wait_time * 1000);
        }
        else if (pfds[0].revents & POLLIN)
        {
        	ICPeer peer;
        	ICMsg *msg = NULL;
        	if (ic.receive(&msg, &peer) < 0)
        	{
        		SELOG("Scheduler::run() -> ic.receive()");
        	}
        	else if (msg)
        	{
        		handle_msg(msg, &peer);
        		delete msg;
        	}
        }
	}
}

void Scheduler::sigchld_handler(int s)
{
	int status, pid;
	if ((pid = wait(&status)) == -1)
	{
		SELOG("Scheduler::sigchld_handler(%d) -> wait", s);
	}
	else
	{
		if (WIFEXITED(status))
		{
			char ret = WEXITSTATUS(status);
			DLOG("Scheduler::sigchld_handler() -> %d ends with %d", pid, ret);
			if (ret != 0)
			{
				ELOG("Scheduler::sigchld_handler() -> %d failed with %d", pid, ret);
			}
		}
		else
		{
			ELOG("Scheduler::sigchld_handler() -> %d ends/hangs with a signal or other event", pid);
		}
	}
}
