
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
