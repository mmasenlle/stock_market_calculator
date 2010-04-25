#ifndef _UTILS_H_
#define _UTILS_H_

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace utils
{
	int strtot(const char *iso8601);
	int today();
	int dec_day(int day);

	bool equald(double a, double b);

	int nohup();
}

#endif
