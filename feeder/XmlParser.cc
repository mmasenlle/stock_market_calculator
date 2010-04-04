
#include "logger.h"
#include "XmlParser.h"

XmlParser::XmlParser()
{

}

void XmlParser::parse(const char *buffer, int len)
{
	DLOG("XmlParser::parse(%p, %d)", buffer, len);
	
}

void XmlParser::store(struct timeval *timestamp)
{

	
	ILOG("XmlParser::store() -> data feed ends successfully");
}
