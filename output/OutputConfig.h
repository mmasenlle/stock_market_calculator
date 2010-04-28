#ifndef _OUTPUTCONFIG_H_
#define _OUTPUTCONFIG_H_

#include "CcltorConfig.h"

enum
{
	OUTPTYPE_ALL,
	OUTPTYPE_OPEN,
	OUTPTYPE_CLOSE,
	OUTPTYPE_COUNT,
	OUTPTYPE_MIN,
	OUTPTYPE_MEAN,
	OUTPTYPE_MAX,
	OUTPTYPE_STD,
	OUTPTYPE_MCOUNT,
	OUTPTYPE_MMIN,
	OUTPTYPE_MMEAN,
	OUTPTYPE_MMAX,
	OUTPTYPE_MSTD,
	OUTPTYPE_YCOUNT,
	OUTPTYPE_YMIN,
	OUTPTYPE_YMEAN,
	OUTPTYPE_YMAX,
	OUTPTYPE_YSTD,
	
	LAST_OUTPTYPE
};

enum
{
	OUTPITEM_PRICE,
	OUTPITEM_VOLUME,
	OUTPITEM_CAPITAL,
	
	LAST_OUTPITEM
};

enum
{
	OUTPMODE_PLOT,
	OUTPMODE_PNG,
	OUTPMODE_HTML,
	
	LAST_OUTPMODE
};

class OutputConfig : public CcltorConfig
{
    void print_help();
    
    void setType(const char *type);
    void setItem(const char *item);
    void setOMode(const char *mode);

public:
	int type, item;
    int day_start, time_start;
    int day_end, time_end;
    std::string value;
    
    int output_mode;
    std::string output_fname;
    std::string tmp_path;
    
    const char *getType() const;
    const char *getItem() const;

    OutputConfig();
    void init(int argc, char *argv[]);
};

#endif
