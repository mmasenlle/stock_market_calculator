
#include "utils.h"
#include "logger.h"
#include "ChartConfig.h"

static const char *chart_types[] = { "all", "close", "min", "mean", "max", NULL };
void ChartConfig::setType(const char *stype)
{
	for (int i = 0; chart_types[i]; i++)
	{
		if (strcasecmp(stype, chart_types[i]) == 0)
		{
			type = i;
			break;
		}
	}
}
const char *ChartConfig::getType()
{
	return chart_types[type];
}

static const char *chart_items[] = { "price", "volume", "capital", NULL };
void ChartConfig::setItem(const char *sitem)
{
	for (int i = 0; chart_items[i]; i++)
	{
		if (strcasecmp(sitem, chart_items[i]) == 0)
		{
			item = i;
			break;
		}
	}	
}
const char *ChartConfig::getItem()
{
	return chart_items[item];
}

ChartConfig::ChartConfig()
{
	type = CHARTTYPE_ALL;
	item = CHARTITEM_PRICE;
	day_start = 0;
	time_start = 0;
	day_end = 20500101;
	time_end = 235959;
    ic_port = 17200;
}

void ChartConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_chart";
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

    FOR_OPT_ARG(argc, argv)
    {
    case 's': day_start = utils::strtot(arg); break;
    case 'S': time_start = utils::strtot(arg); break;
    case 'e': day_end = utils::strtot(arg); break;
    case 'E': time_end = utils::strtot(arg); break;
    case 't': setType(arg); break;
    case 'i': setItem(arg); break;
    case 'V': value = arg; break;
    }
    END_OPT;
    
    if (day_start == 0) // zero means today
    {
    	struct tm lt;
    	time_t tt = time(NULL);
    	localtime_r(&tt, &lt);
    	day_start = ((lt.tm_year + 1900) * 10000) + ((lt.tm_mon + 1) * 100) + lt.tm_mday;
    }

    DLOG("ChartConfig::init() type = %d-%s", type, getType());
    DLOG("ChartConfig::init() item = %d-%s", item, getItem());
    DLOG("ChartConfig::init() day_start = %08d", day_start);
    DLOG("ChartConfig::init() time_start = %06d", time_start);
    DLOG("ChartConfig::init() day_end = %08d", day_end);
    DLOG("ChartConfig::init() time_end = %06d", time_end);
    DLOG("ChartConfig::init() value = '%s'", value.c_str());

	DLOG("ChartConfig::init() ic_port = %d", ic_port);
    DLOG("ChartConfig::init() log_level = %d", log_level);
    DLOG("ChartConfig::init() db_conninfo = '%s'", db_conninfo.c_str());
    DLOG("ChartConfig::init() cfg_fname = '%s'", cfg_fname.c_str());
    DLOG("ChartConfig::init() log_fname = '%s'", log_fname.c_str());
}

void ChartConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_chart [options]\n");
    fprintf(stdout, "Valid types:");
    	for (int i = 0; chart_types[i]; i++) fprintf(stdout, " %s", chart_types[i]); fprintf(stdout, "\n");
    fprintf(stdout, "Valid items:");
    	for (int i = 0; chart_items[i]; i++) fprintf(stdout, " %s", chart_items[i]); fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -s <yyyymmdd>   Starting day\n");
    fprintf(stdout, "  -S <hhmmss>     Starting time\n");
    fprintf(stdout, "  -e <yyyymmdd>   Ending day\n");
    fprintf(stdout, "  -E <hhmmss>     Ending time\n");
    fprintf(stdout, "  -t <type>       Chart type\n");
    fprintf(stdout, "  -i <item>       Chart item\n");
    fprintf(stdout, "  -V <value>      Code of the value\n");
}

