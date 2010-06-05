#ifndef _OUTPUTCONFIG_H_
#define _OUTPUTCONFIG_H_

#include <vector>
#include "CcltorConfig.h"

enum
{
	OUTPTYPE_ALL,

	OUTPTYPE_COUNT,
	OUTPTYPE_OPEN,
	OUTPTYPE_CLOSE,
	OUTPTYPE_MIN,
	OUTPTYPE_MEAN,
	OUTPTYPE_MAX,
	OUTPTYPE_STD,
	OUTPTYPE_MCOUNT,
	OUTPTYPE_MOPEN,
	OUTPTYPE_MCLOSE,
	OUTPTYPE_MMIN,
	OUTPTYPE_MMEAN,
	OUTPTYPE_MMAX,
	OUTPTYPE_MSTD,
	OUTPTYPE_YCOUNT,
	OUTPTYPE_YOPEN,
	OUTPTYPE_YCLOSE,
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
	OUTPTYPE_AD,
	OUTPTYPE_MFI,
	OUTPTYPE_OBV,

	OUTPTYPE_WCOUNT,
	OUTPTYPE_WOPEN,
	OUTPTYPE_WCLOSE,
	OUTPTYPE_WMIN,
	OUTPTYPE_WMEAN,
	OUTPTYPE_WMAX,
	OUTPTYPE_WSTD,
	OUTPTYPE_WP,
	OUTPTYPE_WR1,
	OUTPTYPE_WS1,
	OUTPTYPE_WR2,
	OUTPTYPE_WS2,
	OUTPTYPE_WR3,
	OUTPTYPE_WS3,
	OUTPTYPE_WR4,
	OUTPTYPE_WS4,
	OUTPTYPE_WMF,
	OUTPTYPE_WSMA,
	OUTPTYPE_WMAD,
	OUTPTYPE_WCCI,
	OUTPTYPE_WROC,
	OUTPTYPE_WAD,
	OUTPTYPE_WMFI,
	OUTPTYPE_WOBV,
	
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

	int normalize;
    int output_mode;
    std::string output_fname;
    std::string tmp_path;
    
    const char *getType() const;
    const char *getItem() const;

    OutputConfig();
    void init(int argc, char *argv[]);
};

#endif
