
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>
#include "XPathConfig.h"

XPathConfig::XPathConfig(const char *arg, int len)
 : doc(NULL), xpathCtx(NULL)
{
	xmlInitParser();
	LIBXML_TEST_VERSION
	if (!len)
	{
		if (access(arg, R_OK) == 0)
		{
			doc = xmlParseFile(arg);
		}
	}
	else
	{
		doc = xmlParseMemory(arg, len);
	}
	if (doc)
	{
		xpathCtx = xmlXPathNewContext(doc);
	}
}

bool XPathConfig::ok() const
{
	return (xpathCtx != NULL);
}

XPathConfig::~XPathConfig()
{
	if (xpathCtx)
	{
		xmlXPathFreeContext(xpathCtx);
	}
	if (doc)
	{
		xmlFreeDoc(doc);
	}
	xmlCleanupParser();
}

int XPathConfig::getValue(const char *key, std::string *val, int ins) const
{  
    xmlXPathObjectPtr xpathObj; 

    if (!doc || !xpathCtx)
    {
//    	fprintf(stderr, "XPathConfig::getValue -> File not loaded trying %s(%d)", key, ins); 
        return 0;
    }
    
    xpathObj = xmlXPathEvalExpression((const xmlChar*)key, xpathCtx);
    if (xpathObj == NULL)
    {
//    	fprintf(stderr, "XPathConfig::getValue -> Unable to evaluate xpath expression \"%s\"", key);
        return 0;
    }

    if (xpathObj->nodesetval == NULL)
    {
        xmlXPathFreeObject(xpathObj);
        return 0;
    }
    
    int count = xpathObj->nodesetval->nodeNr;
    if (count <= ins)
    {
    	xmlXPathFreeObject(xpathObj);
        return 0;
    }
    
    if (val)
    {
    	xmlChar *xc = xmlNodeGetContent(xpathObj->nodesetval->nodeTab[ins]);
	    if (!xc || !(*xc))
	    {
	        xmlXPathFreeObject(xpathObj);
	        return 0;
	    }
    	*val = (const char*)xc;
    	xmlFree(xc);
    }
     
    xmlXPathFreeObject(xpathObj);
    return count;
}
