#ifndef _CHARTCONFIG_H_
#define _CHARTCONFIG_H_

#include "CcltorConfig.h"

enum
{
	CHARTTYPE_NORMAL,
	CHARTTYPE_MIN,
	CHARTTYPE_MEAN,
	CHARTTYPE_MAX,
	
	LAST_CHARTTYPE
};

enum
{
	CHARTITEM_PRICE,
	CHARTITEM_VOLUME,
	CHARTITEM_CAPITAL,
	
	LAST_CHARTITEM
};

class ChartConfig : public CcltorConfig
{
    void print_help();
    
    void setType(const char *type);
    void setItem(const char *item);

public:
	int type, item;
    int day_start, time_start;
    int day_end, time_end;
    std::string value;
    
    const char *getType();
    const char *getItem();

	ChartConfig();
    void init(int argc, char *argv[]);
};

#endif /*_CHARTCONFIG_H_*/
