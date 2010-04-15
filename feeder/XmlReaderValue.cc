
#include <stdlib.h>
#include "utils.h"
#include "logger.h"
#include "Feeder.h"
#include "XmlReaderValue.h"

#define CAPITAL		"capital"
#define CODE		"code"
#define NAME		"name"
#define PRICE		"price"
#define TIME		"time"
#define VOLUME		"volume"

static const char *value_labels[] = {
/* sorted !!! */
	CAPITAL,
	CODE,
	NAME,
	PRICE,
	TIME,
	VOLUME,
};

const char * XmlReaderValue::match_value(int hint)
{
	for (int i = 0; i < ARRAY_SIZE(value_labels); i++)
	{
		if (value_labels[i][0] < hint)
			continue;
		if (value_labels[i][0] > hint)
			return NULL;
		if (match_string(value_labels[i]))
			return value_labels[i];
	}
	return NULL;
}

XmlReader * XmlReaderValue::match_tree(int hint)
{
	return NULL;
}

void XmlReaderValue::done()
{
	std::string code;
	if (values.find(CODE) == values.end() || values[CODE].size() != 1)
	{
		WLOG("XmlReaderValue::done() -> NO code (or several) in the value");
		return;
	}
	code = values[CODE][0];
	int hhmmss_time = 0;	if (values.find(TIME) == values.end() || values[TIME].size() != 1)
	{
		WLOG("XmlReaderValue::done(%s) -> NO time (or several) in the value", code.c_str());
		return;
	}
	else
	{
		int h, m;
		if (sscanf(values[TIME][0].c_str(), "%d:%d", &h, &m) == 2)
		{
			hhmmss_time = ((h * 100) + m) * 100;
		}
		else
		{
			if (sscanf(values[TIME][0].c_str(), "%d/%d", &h, &m) == 2)
			{
				DLOG("XmlReaderValue::done(%s) -> '%s' not today's value", code.c_str(), values[TIME][0].c_str());
			}
			else
			{
				WLOG("XmlReaderValue::done(%s) -> time format '%s' not understood", code.c_str(), values[TIME][0].c_str());
			}
			return;
		}
	}
	std::string name;
	if (values.find(NAME) == values.end() || values[NAME].size() != 1)
	{
		WLOG("XmlReaderValue::done(%s) -> NO name (or several) in the value", code.c_str());
	}
	else
	{
		name = values[NAME][0];
	}
	double price = 0.0;
	if (values.find(PRICE) == values.end() || values[PRICE].size() != 1)
	{
		WLOG("XmlReaderValue::done(%s) -> NO price (or several) in the value", code.c_str());
	}
	else
	{
		price = strtod(values[PRICE][0].c_str(), NULL);
	}
	double volume = 0.0;
	if (values.find(VOLUME) == values.end() || values[VOLUME].size() != 1)
	{
		WLOG("XmlReaderValue::done(%s) -> NO volume (or several) in the value", code.c_str());
	}
	else
	{
		volume = strtod(values[VOLUME][0].c_str(), NULL);
	}
	double capital = 0.0;
	if (values.find(CAPITAL) == values.end() || values[CAPITAL].size() != 1)
	{
		WLOG("XmlReaderValue::done(%s) -> NO capital (or several) in the value", code.c_str());
	}
	else
	{
		capital = strtod(values[CAPITAL][0].c_str(), NULL);
	}

	Feeder::feeder.dbfeeder.insert_price(code.c_str(), name.c_str(), price, volume, capital, hhmmss_time);	DLOG("XmlReaderValue::done(%s) -> name='%s' price=%f volume=%f capital=%f time=%06d",
	    code.c_str(), name.c_str(), price, volume, capital, hhmmss_time);
}
