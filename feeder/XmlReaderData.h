#ifndef _XMLREADERDATA_H_
#define _XMLREADERDATA_H_

#include "XmlReader.h"

class XmlReaderData : public XmlReader
{
	const char * match_value(int hint);
	XmlReader * match_tree(int hint);
	void done();
public:
	XmlReaderData();
};

#endif /*_XMLREADERDATA_H_*/
