#ifndef _XPATHCONFIG_H_
#define _XPATHCONFIG_H_

#include <string>
#include <libxml/tree.h>
#include <libxml/xpath.h>

class XPathConfig
{
	xmlDocPtr doc;
	xmlXPathContextPtr xpathCtx;
public:
	XPathConfig(const char *arg, int len = 0);
	~XPathConfig();
	int getValue(const char *key, std::string *val, int ins = 0) const;
	bool ok() const;
};

#endif

