
#include <signal.h>
#include <stdlib.h>
#include "utils.h"

int utils::strtot(const char *iso8601)
{
	char *p;
	int t = strtol(iso8601, &p, 10);
	if(p && *p)
	{
		t = (t * 100) + strtol(++p, &p, 10);
		if(p && *p)
		{
			t = (t * 100) + strtol(++p, NULL, 10);
		}
	}
	return t;
}

int utils::nohup()
{
	int ret = 0;
	if (signal(SIGHUP, SIG_IGN) == SIG_ERR) ret--;
	if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) ret--;
	if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) ret--;
	if (signal(SIGTTIN, SIG_IGN) == SIG_ERR) ret--;
	return ret;
}
