
#include "logger.h"
#include "XmlReaderValue.h"
#include "XmlReaderData.h"

#define DATA_ROOT_LABEL "ccltor_xml_data"

XmlReaderData::XmlReaderData() : XmlReader(DATA_ROOT_LABEL) {};

const char * XmlReaderData::match_value(int hint)
{
	return NULL;
}

XmlReader * XmlReaderData::match_tree(int hint)
{
	if (match_string(VALUE_ROOT_LABEL))
		return new XmlReaderValue;

	return NULL;
}

int XmlReaderData::get_count()
{
	return (trees.find(VALUE_ROOT_LABEL) == trees.end()) ? 0 : trees[VALUE_ROOT_LABEL].size();}
