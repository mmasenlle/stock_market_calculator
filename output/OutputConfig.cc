
#include "utils.h"
#include "logger.h"
#include "OutputConfig.h"

static const char *outp_types[] = { "all",
		"count", "open", "close", "min", "mean", "max", "std",
		"mcount", "mopen", "mclose", "mmin", "mmean", "mmax", "mstd",
		"ycount", "yopen", "yclose", "ymin", "ymean", "ymax", "ystd",
		"P", "R1", "S1", "R2", "S2", "R3", "S3", "R4", "S4", "MF",
		"SMA", "MAD", "CCI", "ROC", "AD", "MFI", "OBV",
		"wcount", "wopen", "wclose", "wmin", "wmean", "wmax", "wstd",
		"wP", "wR1", "wS1", "wR2", "wS2", "wR3", "wS3", "wR4", "wS4", "wMF",
		"wSMA", "wMAD", "wCCI", "wROC", "wAD", "wMFI", "wOBV",
		"iPP",
		NULL };
void OutputConfig::setType(const char *stype, OutpDesc *odesc)
{
	for (int i = 0; outp_types[i]; i++)
	{
		if (strcasecmp(stype, outp_types[i]) == 0)
		{
			odesc->type = i;
			break;
		}
	}
}
const char *OutputConfig::getType() const
{
	return outp_types[outpdesc.type];
}

static const char *outp_items[] = { "price", "volume", "capital", NULL };
void OutputConfig::setItem(const char *sitem, OutpDesc *odesc)
{
	for (int i = 0; outp_items[i]; i++)
	{
		if (strcasecmp(sitem, outp_items[i]) == 0)
		{
			odesc->item = i;
			break;
		}
	}	
}
const char *OutputConfig::getItem() const
{
	return outp_items[outpdesc.item];
}

static const char *outp_modes[] = { "plot", "png", "html", NULL };
void OutputConfig::setOMode(const char *mode)
{
	for (int i = 0; outp_modes[i]; i++)
	{
		if (strcasecmp(mode, outp_modes[i]) == 0)
		{
			output_mode = i;
			break;
		}
	}	
}

OutputConfig::OutputConfig()
{
	outpdesc.type = OUTPTYPE_ALL;
	outpdesc.item = OUTPITEM_PRICE;
	outpdesc.day_start = 0;
	outpdesc.time_start = 0;
	outpdesc.day_end = 20500101;
	outpdesc.time_end = 235959;
	output_mode = OUTPMODE_PLOT;
	tmp_path = "/tmp";
    ic_port = 17200;
	normalize = -1;
}

void OutputConfig::setOutpDescs(const char *multiple)
{
	DLOG("OutputConfig::setOutpDescs(%s)", multiple);
	char *tok, *p = strtok_r((char *)multiple, ",", &tok);
	while (p) 
	{
		OutpDesc odesc = outpdesc;
		char *q = p;
		for (int i = 0; q; i++, p = q + 1)
		{
			q = strchr(p, ':');
			if (q) *q = 0;
			switch (i)
			{
			case 0: if (*p) setType(p, &odesc); continue;
			case 1: if (*p) setItem(p, &odesc); continue;
			case 2: if (*p) odesc.value = p; continue;
			case 3: if (*p) odesc.day_start = utils::strtot(p); continue;
			case 4: if (*p) odesc.time_start = utils::strtot(p); continue;
			case 5: if (*p) odesc.day_end = utils::strtot(p); continue;
			case 6: if (*p) odesc.time_end = utils::strtot(p); continue;
			}
			break;
		}
		if (outpdescs.empty() || odesc.type != outpdesc.type || odesc.item != outpdesc.item || 
		    odesc.value != outpdesc.value ||
		    odesc.day_start != outpdesc.day_start || odesc.time_start != outpdesc.time_start ||
		    odesc.day_end != outpdesc.day_end || odesc.time_end != outpdesc.time_end)
		{
			DLOG("OutputConfig::setOutpDescs(%d) -> %d:%d:%s:%d:%d:%d:%d", outpdescs.size(),
			    odesc.type, odesc.item, odesc.value.c_str(), odesc.day_start, odesc.time_start,
			    odesc.day_end, odesc.time_end);
			outpdescs.push_back(odesc);
		}
		p = strtok_r(NULL, ",", &tok);
	}
}

void OutputConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_output";
    XPathConfig xpconf(cfg_fname.c_str());

	init_post(xpconf, key.c_str(), argc, argv);

    if (xpconf.getValue((key + "/type").c_str(), &xp_value))
    	setType(xp_value.c_str(), &outpdesc);
    if (xpconf.getValue((key + "/item").c_str(), &xp_value))
    	setItem(xp_value.c_str(), &outpdesc);
    if (xpconf.getValue((key + "/day_start").c_str(), &xp_value))
    	outpdesc.day_start = utils::strtot(xp_value.c_str());
    if (xpconf.getValue((key + "/time_start").c_str(), &xp_value))
    	outpdesc.time_start = utils::strtot(xp_value.c_str());
    if (xpconf.getValue((key + "/day_end").c_str(), &xp_value))
    	outpdesc.day_end = utils::strtot(xp_value.c_str());
    if (xpconf.getValue((key + "/time_end").c_str(), &xp_value))
    	outpdesc.time_end = utils::strtot(xp_value.c_str());
    xpconf.getValue((key + "/value").c_str(), &outpdesc.value);
    if (xpconf.getValue((key + "/multiple").c_str(), &xp_value))
    	setOutpDescs(xp_value.c_str());
	if (xpconf.getValue((key + "/normalize").c_str(), &xp_value))
    	normalize = atoi(xp_value.c_str());
    
    if (xpconf.getValue((key + "/output_mode").c_str(), &xp_value))
    	setOMode(xp_value.c_str());
    xpconf.getValue((key + "/output_fname").c_str(), &output_fname);
    xpconf.getValue((key + "/tmp_path").c_str(), &tmp_path);

	FOR_OPT(argc, argv)
    {
    case 'n': normalize = 0; break;
	}
	END_OPT;
    FOR_OPT_ARG(argc, argv)
    {
    case 's': outpdesc.day_start = utils::strtot(arg); break;
    case 'S': outpdesc.time_start = utils::strtot(arg); break;
    case 'e': outpdesc.day_end = utils::strtot(arg); break;
    case 'E': outpdesc.time_end = utils::strtot(arg); break;
    case 't': setType(arg, &outpdesc); break;
    case 'i': setItem(arg, &outpdesc); break;
    case 'V': outpdesc.value = arg; break;
	case 'n': normalize = atoi(arg); break;
    case 'o': setOMode(arg); break;
    case 'O': output_fname = arg; break;
    case 'P': tmp_path = arg; break;
    }
    END_OPT;
    if (outpdesc.day_start > -50 && outpdesc.day_start <= 0) // zero means today
    {
    	int i = outpdesc.day_start;
    	outpdesc.day_start = utils::today();
    	while (i++) outpdesc.day_start = utils::dec_day(outpdesc.day_start);
    }
    FOR_OPT_ARG(argc, argv)
    {
    case 'm': setOutpDescs(arg); break;
    }
    END_OPT;

    DLOG("OutputConfig::init() type = %d-%s", outpdesc.type, getType());
    DLOG("OutputConfig::init() item = %d-%s", outpdesc.item, getItem());
    DLOG("OutputConfig::init() day_start = %08d", outpdesc.day_start);
    DLOG("OutputConfig::init() time_start = %06d", outpdesc.time_start);
    DLOG("OutputConfig::init() day_end = %08d", outpdesc.day_end);
    DLOG("OutputConfig::init() time_end = %06d", outpdesc.time_end);
    DLOG("OutputConfig::init() value = '%s'", outpdesc.value.c_str());
	DLOG("OutputConfig::init() normalize = %d", normalize);
    DLOG("OutputConfig::init() output_mode = %d-%s", output_mode, outp_modes[output_mode]);
    DLOG("OutputConfig::init() output_fname = '%s'", output_fname.c_str());
    DLOG("OutputConfig::init() tmp_path = '%s'", tmp_path.c_str());
    DLOG("OutputConfig::init() outpdescs.size() = %d", outpdescs.size());
    
	DLOG("OutputConfig::init() ic_port = %d", ic_port);
    DLOG("OutputConfig::init() log_level = %d", log_level);
    DLOG("OutputConfig::init() db_conninfo = '%s'", db_conninfo.c_str());
    DLOG("OutputConfig::init() cfg_fname = '%s'", cfg_fname.c_str());
    DLOG("OutputConfig::init() log_fname = '%s'", log_fname.c_str());
}

void OutputConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_output [options]\n");
    fprintf(stdout, "Valid types:");
    	for (int i = 0; outp_types[i]; i++) fprintf(stdout, " %s", outp_types[i]); fprintf(stdout, "\n");
    fprintf(stdout, "Valid items:");
    	for (int i = 0; outp_items[i]; i++) fprintf(stdout, " %s", outp_items[i]); fprintf(stdout, "\n");
    fprintf(stdout, "Valid output modes:");
    	for (int i = 0; outp_modes[i]; i++) fprintf(stdout, " %s", outp_modes[i]); fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -s <yyyymmdd>   Starting day\n");
    fprintf(stdout, "  -S <hhmmss>     Starting time\n");
    fprintf(stdout, "  -e <yyyymmdd>   Ending day\n");
    fprintf(stdout, "  -E <hhmmss>     Ending time\n");
    fprintf(stdout, "  -t <type>       Output data type\n");
    fprintf(stdout, "  -i <item>       Output data item\n");
    fprintf(stdout, "  -V <value>      Code of the value\n");
	fprintf(stdout, "  -n [<value>]    Normalize values to the first one or value\n");
    fprintf(stdout, "  -o <mode>       Output mode\n");
    fprintf(stdout, "  -O <file>       Ouput file name\n");
    fprintf(stdout, "  -P <path>       Path for the temporal files\n");
    fprintf(stdout, "  -m <multiple>   Multiple output descriptor ([type1][:[item1][:[value1[:[s1][:[S1]]]]]][,[type2] ...])\n");
}

