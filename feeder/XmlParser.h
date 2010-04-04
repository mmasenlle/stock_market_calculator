#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include <sys/time.h>

class XmlParser
{

public:
	XmlParser();
	void parse(const char *buffer, int len);
	void store(struct timeval *timestamp);
};

#endif
