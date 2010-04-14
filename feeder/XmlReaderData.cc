
#include "logger.h"
#include "feeder.h"
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

void XmlReaderData::done()
{	if (trees.find(VALUE_ROOT_LABEL) == trees.end())
	{
		dbfeeder.insert_feed(feeder_id, 0);
		WLOG("XmlReaderData::done() -> NO values processed at this feed");
	}
	else
	{
		dbfeeder.insert_feed(feeder_id, trees[VALUE_ROOT_LABEL].size());
		ILOG("XmlReaderData::done() -> %d values processed at this feed", trees[VALUE_ROOT_LABEL].size());
	}
}
