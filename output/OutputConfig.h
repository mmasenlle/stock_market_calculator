#ifndef _OUTPUTCONFIG_H_
#define _OUTPUTCONFIG_H_

#include <vector>
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
	OUTPTYPE_P,
	OUTPTYPE_R1,
	OUTPTYPE_S1,
	OUTPTYPE_R2,
	OUTPTYPE_S2,
	OUTPTYPE_R3,
	OUTPTYPE_S3,
	OUTPTYPE_R4,
	OUTPTYPE_S4,
	OUTPTYPE_MF,
	OUTPTYPE_SMA,
	OUTPTYPE_MAD,
	OUTPTYPE_CCI,
	OUTPTYPE_ROC,
	OUTPTYPE_MFI,
	
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

struct OutpDesc
{
	int type, item;
	int day_start, time_start;
	int day_end, time_end;
	std::string value;
};

class OutputConfig : public CcltorConfig
{
    void print_help();
    
    void setType(const char *type, OutpDesc *odesc);
    void setItem(const char *item, OutpDesc *odesc);
    void setOMode(const char *mode);
    
    void setOutpDescs(const char *multiple);

public:
    OutpDesc outpdesc;
    std::vector<OutpDesc> outpdescs;
    
    int output_mode;
    std::string output_fname;
    std::string tmp_path;
    
    const char *getType() const;
    const char *getItem() const;

    OutputConfig();
    void init(int argc, char *argv[]);
};

#endif
