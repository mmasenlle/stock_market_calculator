
#include <time.h>
#include <signal.h>
#include <math.h>
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

int utils::today()
{
	struct tm lt;
	time_t tt = time(NULL);
	localtime_r(&tt, &lt);
	return ((lt.tm_year + 1900) * 10000) + ((lt.tm_mon + 1) * 100) + lt.tm_mday;
}

static const int month_days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int utils::dec_day(int day)
{
	if (--day % 100 == 0)
	{
		if ((day -= 100) % 10000 == 0)
			return (day - 8769);

		day += month_days[(day / 100) % 100];
		if ((day % 100 == 28) && ((day / 10000) % 4) == 0)
			day++;
	}
	return day;
}

bool utils::equald(double a, double b)
{
#define EQUALD_PRECISION 1e-12
	return (a == b || fabs(a - b) < fabs(a * EQUALD_PRECISION));
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
