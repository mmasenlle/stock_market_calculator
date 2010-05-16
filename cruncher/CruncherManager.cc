#include <signal.h>
#include <poll.h>
#include <dlfcn.h>
#include "utils.h"
#include "logger.h"
#include "ICRegistration.h"
#include "ICEvent2.h"
#include "CruncherManager.h"


int CruncherManager::observe(int event)
{
	DLOG("CruncherManager::observe(%d) -> pid: %d", event, getpid());
	pthread_mutex_lock(&manager_mtx);
	observers[event].insert(getpid());
	pthread_mutex_unlock(&manager_mtx);
	return 0;
}

int CruncherManager::send(ICMsg *msg, const ICPeer *peer)
{
	if (peer)
		return msg->send(&ic, peer);

	handle_msg(msg, NULL);
	return 0;
}

int CruncherManager::cruncher_fn(void *arg)
{
	Cruncher *cruncher = (Cruncher *)arg;
	DLOG("CruncherManager::cruncher_fn(%d) -> cruncher: %p", cruncher->pid, cruncher);
	pthread_mutex_lock(&cruncher->mtx);
	pthread_mutex_unlock(&cruncher->mtx);
	cruncher->cruncher->run();
	WLOG("CruncherManager::cruncher_fn(%d) -> returned", cruncher->pid);
	return 1;
}

CruncherManager::CruncherManager()
{
	ccfg = &config;
	cache = &dbcache;
}

void CruncherManager::init()
{
	int r = pthread_mutex_init(&manager_mtx, NULL);
	if (r != 0)
	{
		ELOG("CruncherManager::init() -> pthread_mutex_init(manager): %d", r);
		return;
	}
	if ((r = utils::nohup()) < 0)
	{
		SELOG("CruncherManager::init() -> utils::nohup(): %d", r);
	}
	if (ic.init(config.ic_port) == -1)
	{
		SELOG("CruncherManager::init() -> ic.init(%d)", config.ic_port);
	}
	dbcache.init(config.db_conninfo.c_str());

	for (int i = 0; i < config.plugins.size(); i++)
	{
		ICruncher *icruncher = NULL;
		Cruncher *cruncher = NULL;
		std::string plugin_fname = config.plugins_path + "/libCC" +	config.plugins[i] + ".so";
		void *handle = dlopen(plugin_fname.c_str(), RTLD_LAZY);
		if (!handle)
		{
			ELOG("CruncherManager::init(%d) -> dlopen(%s): %s", i, plugin_fname.c_str(), dlerror());
			continue;
		}
		getInstance_t getInstance = (getInstance_t)dlsym(handle, CRUNCHER_GETINSTANCE_SYMBOL);
		if (!getInstance)
		{
			ELOG("CruncherManager::init(%d) -> dlsym(getInstance): %s", i, dlerror());
			goto plugin_error;
		}
		if (!(icruncher = getInstance()))
		{
			ELOG("CruncherManager::init(%d) -> getInstance(): NULL", i);
			goto plugin_error;
		}
		cruncher = new Cruncher;
		cruncher->cruncher = icruncher;
		if ((r = pthread_mutex_init(&cruncher->mtx, NULL)) != 0)
		{
			ELOG("CruncherManager::init(%d) -> pthread_mutex_init(cruncher): %d", i, r);
			goto plugin_error;
		}
		pthread_mutex_lock(&cruncher->mtx);
		cruncher->pid = clone(cruncher_fn, cruncher->stack + sizeof(cruncher->stack) - sizeof(void*), 
				CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_PTRACE | CLONE_VM, cruncher);
		if (cruncher->pid == -1)
		{
			SELOG("CruncherManager::init(%d) -> clone", i);
			goto plugin_error;
		}
		crunchers[cruncher->pid] = cruncher;
		r = icruncher->init(this);
		ILOG("CruncherManager::init(%d, %d) -> icruncher->init(): %d", i, cruncher->pid, r);
		continue;

plugin_error:
		delete icruncher;
		delete cruncher;
		dlclose(handle);
	}
}

void CruncherManager::handle_msg(ICMsg *msg, ICPeer *from)
{
	DLOG("CruncherManager::handle_msg(%d)", msg->getClass());
	switch (msg->getClass())
	{
	case ICMSGCLASS_EVENT:
	{
		ICEvent *ev = (ICEvent *)msg;
		ICEvent2 *ev2 = (ev->getEvent() > ICEVENT_EVENT2) ? (ICEvent2 *)ev : NULL;
		switch (ev->getEvent())
		{
		case ICEVENT_CONTROL_RUNNING:
			ILOG("CruncherManager::handle_msg(RUNNING:%d)", ev2->getSubEvent());
			switch (ev2->getSubEvent())
			{
			case ICEVENT2_CONTROL_RUNNING_PING:
			{
				ICEvent2 ack(ICEVENT_CONTROL_RUNNING, ICEVENT2_CONTROL_RUNNING_ACK);
				ack.send(&ic, from);
				break;
			}
			case ICEVENT2_CONTROL_RUNNING_STOP:
				for (std::map<int, Cruncher *>::iterator i = crunchers.begin(); i != crunchers.end(); i++)
					kill(i->first, SIGTERM);
				exit(0);
			case ICEVENT2_CONTROL_RUNNING_RECONNECT:
				break;
			}
			break;
		case ICEVENT_CONTROL_LOGLEVEL:
			DLOG("CruncherManager::handle_msg(LOGLEVEL:%d)", ev2->getSubEvent());
			SET_LOGGER_LEVEL(ev2->getSubEvent());
			break;
		default:
			pthread_mutex_lock(&manager_mtx);
			if (observers.find(ev->getEvent()) != observers.end())
			{
				for (std::set<int>::iterator i = observers[ev->getEvent()].begin();
				i != observers[ev->getEvent()].end(); i++)
				{
					crunchers[*i]->cruncher->msg(ev);
				}
			}
			else
			{
				WLOG("CruncherManager::handle_msg() -> unhandled event %d", ev->getEvent());
			}
			pthread_mutex_unlock(&manager_mtx);
		}
		break;
	}
	default:
		WLOG("CruncherManager::handle_msg() -> NOT expected msg class %d", msg->getClass());
	}
}

void CruncherManager::handle_ic()
{
	ICPeer peer;
	ICMsg *msg = NULL;
	if (ic.receive(&msg, &peer) < 0)
	{
		SELOG("CruncherManager::handle_ic() -> ic.receive()");
	}
	else if (msg)
	{
		handle_msg(msg, &peer);
		delete msg;
	}
}

#define CRUNCHERMANAGER_WAITTIME 500000

void CruncherManager::run()
{
	DLOG("CruncherManager::run() -> crunchers.size(): %d", crunchers.size());
	for (std::map<int, Cruncher *>::iterator i = crunchers.begin(); i != crunchers.end(); i++)
	{
		pthread_mutex_unlock(&i->second->mtx);
	}
	struct pollfd pfd;
	pfd.fd = ic.get_fd();
	pfd.events = POLLIN;
	for (;;)
	{
		if (poll(&pfd, 1, CRUNCHERMANAGER_WAITTIME) == -1 && errno != EINTR)
		{
			SELOG("CruncherManager::run() -> poll(%d, %d)", pfd.fd, CRUNCHERMANAGER_WAITTIME);
		}
		else if (pfd.revents & POLLIN)
		{
			handle_ic();
		}
		else // FIXME: do it more intelligently
		{
			DLOG("CruncherManager::run() -> nothing happened in the last %d ms", CRUNCHERMANAGER_WAITTIME);
			dbcache.drain();
			if (observers.find(ICEVENT_FEEDER_NEWFEED) != observers.end())
			{
				ICRegistration registration(CRUNCHERMANAGER_WAITTIME);
			    for (int i = 0; i < config.feeders.size(); i++)
			    {
			    	if (registration.send(&ic, &config.feeders[i]) <= 0)
			    	{
			    		SELOG("CruncherManager::run() -> registration.send(ICEVENT_FEEDER_NEWFEED)");
			    	}
			    }
			}
		}
		
		// TODO: refresh more registerings ?
	}
}
