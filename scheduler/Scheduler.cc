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

void Scheduler::update()
{
	// walk config and update next, delay and next_time
	next = -1;
	for (int i = 0; i < config.cmds.size(); i++)
	{
		
	}
}

void Scheduler::exec()
{
	// run next
}

void Scheduler::init()
{
	if (utils::nohup() < 0)
	{
		SELOG("Scheduler::init() -> utils::nohup()");
	}
	if (ic.init(config.ic_port) == -1)
	{
		SELOG("Scheduler::init() -> ic.init(%d)", config.ic_port);
	}
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

	update();

	for (;;)
	{
		int wait_time = -1;
		while (next >= 0)
		{
			wait_time = next_time - time(NULL);
			if (wait_time > 0 && wait_time <= delay)
				break;
			exec();
			update();
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
