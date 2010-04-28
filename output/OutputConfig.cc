
#include "utils.h"
#include "logger.h"
#include "OutputConfig.h"

static const char *outp_types[] = { "all", "open", "close", "count", "min", "mean", "max", "std",
		"mcount", "mmin", "mmean", "mmax", "mstd", "ycount", "ymin", "ymean", "ymax", "ystd", NULL };
void OutputConfig::setType(const char *stype)
{
	for (int i = 0; outp_types[i]; i++)
	{
		if (strcasecmp(stype, outp_types[i]) == 0)
		{
			type = i;
			break;
		}
	}
}
const char *OutputConfig::getType() const
{
	return outp_types[type];
}

static const char *outp_items[] = { "price", "volume", "capital", NULL };
void OutputConfig::setItem(const char *sitem)
{
	for (int i = 0; outp_items[i]; i++)
	{
		if (strcasecmp(sitem, outp_items[i]) == 0)
		{
			item = i;
			break;
		}
	}	
}
const char *OutputConfig::getItem() const
{
	return outp_items[item];
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
	type = OUTPTYPE_ALL;
	item = OUTPITEM_PRICE;
	day_start = 0;
	time_start = 0;
	day_end = 20500101;
	time_end = 235959;
	output_mode = OUTPMODE_PLOT;
	tmp_path = "/tmp";
    ic_port = 17200;
}

void OutputConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_output";
    XPathConfig xpconf(cfg_fname.c_str());

	init_post(xpconf, key.c_str(), argc, argv);

    if (xpconf.getValue((key + "/type").c_str(), &xp_value))
    	setType(xp_value.c_str());
    if (xpconf.getValue((key + "/item").c_str(), &xp_value))
    	setItem(xp_value.c_str());
    if (xpconf.getValue((key + "/day_start").c_str(), &xp_value))
    	day_start = utils::strtot(xp_value.c_str());
    if (xpconf.getValue((key + "/time_start").c_str(), &xp_value))
    	time_start = utils::strtot(xp_value.c_str());
    if (xpconf.getValue((key + "/day_end").c_str(), &xp_value))
    	day_end = utils::strtot(xp_value.c_str());
    if (xpconf.getValue((key + "/time_end").c_str(), &xp_value))
    	time_end = utils::strtot(xp_value.c_str());
    xpconf.getValue((key + "/value").c_str(), &value);
    
    if (xpconf.getValue((key + "/output_mode").c_str(), &xp_value))
    	setOMode(xp_value.c_str());
    xpconf.getValue((key + "/output_fname").c_str(), &output_fname);
    xpconf.getValue((key + "/tmp_path").c_str(), &tmp_path);

    FOR_OPT_ARG(argc, argv)
    {
    case 's': day_start = utils::strtot(arg); break;
    case 'S': time_start = utils::strtot(arg); break;
    case 'e': day_end = utils::strtot(arg); break;
    case 'E': time_end = utils::strtot(arg); break;
    case 't': setType(arg); break;
    case 'i': setItem(arg); break;
    case 'V': value = arg; break;
    case 'o': setOMode(arg); break;
    case 'O': output_fname = arg; break;
    case 'P': tmp_path = arg; break;
    }
    END_OPT;
    
    if (day_start == 0) // zero means today
		day_start = utils::today();

    DLOG("OutputConfig::init() type = %d-%s", type, getType());
    DLOG("OutputConfig::init() item = %d-%s", item, getItem());
    DLOG("OutputConfig::init() day_start = %08d", day_start);
    DLOG("OutputConfig::init() time_start = %06d", time_start);
    DLOG("OutputConfig::init() day_end = %08d", day_end);
    DLOG("OutputConfig::init() time_end = %06d", time_end);
    DLOG("OutputConfig::init() value = '%s'", value.c_str());
    DLOG("OutputConfig::init() output_mode = %d-%s", output_mode, outp_modes[output_mode]);
    DLOG("OutputConfig::init() output_fname = '%s'", output_fname.c_str());
    DLOG("OutputConfig::init() tmp_path = '%s'", tmp_path.c_str());
    
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
    fprintf(stdout, "  -o <mode>       Output mode\n");
    fprintf(stdout, "  -O <file>       Ouput file name\n");
    fprintf(stdout, "  -P <path>       Path for the temporal files\n");
}
