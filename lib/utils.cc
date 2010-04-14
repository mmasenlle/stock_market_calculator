
#include <stdlib.h>
#include "utils.h"

int utils::strtot(const char *iso8601)
{
	char *p;
	int t = strtol(iso8601, &p, 10);
	t *= 100;
	if(p) t += strtol(++p, &p, 10);
	t *= 100;
	if(p) t += strtol(++p, NULL, 10);
	return t;
}
