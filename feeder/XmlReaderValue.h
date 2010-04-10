#ifndef _XMLREADERVALUE_H_
#define _XMLREADERVALUE_H_

#include "XmlReader.h"

#define VALUE_ROOT_LABEL "value"

class XmlReaderValue : public XmlReader
{
	const char * match_value(int hint);
	XmlReader * match_tree(int hint);
	void done();
public:
	XmlReaderValue() : XmlReader(VALUE_ROOT_LABEL) {};
};

#endif /*_XMLREADERVALUE_H_*/
