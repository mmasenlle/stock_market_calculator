
#include "logger.h"
#include "FeederConfig.h"

FeederConfig::FeederConfig()
{
    sdelay = 5 * 60;
    days_off = 0;
    time_start = 0;
    time_stop = 24 * 60;
    ic_port = 17100;
}

void FeederConfig::init(int argc, char *argv[])
{
    init_pre(argc, argv);

    std::string xp_value, key = "/ccltor_feeder";
    XPathConfig xpconf(cfg_fname.c_str());
    if (xpconf.getValue((key + "/sdelay").c_str(), &xp_value))
        sdelay = atoi(xp_value.c_str());
    if (xpconf.getValue((key + "/days_off").c_str(), &xp_value))
    {
        days_off = 0;
        for (int i = 0; i < xp_value.length() && i < 7; i++)
            if (xp_value[i] == '1') days_off |= (1 << i);
    }
    if (xpconf.getValue((key + "/time_start").c_str(), &xp_value))
    {
        int h, m;
        if (sscanf(xp_value.c_str(), "%d:%d", &h, &m) == 2)
            time_start = (h * 60) + m;
    }
    if (xpconf.getValue((key + "/time_stop").c_str(), &xp_value))
    {
        int h, m;
        if (sscanf(xp_value.c_str(), "%d:%d", &h, &m) == 2)
            time_stop = (h * 60) + m;
    }
    xpconf.getValue((key + "/url").c_str(), &url);
    xpconf.getValue((key + "/parser").c_str(), &parser);

    FOR_OPT_ARG(argc, argv)
    {
    case 'd': sdelay = atoi(arg); break;
    case 'u': url = arg; break;
    case 'p': parser = arg; break;
    }
    END_OPT;

    init_post(xpconf, key.c_str(), argc, argv);

    DLOG("FeederConfig::init() sdelay = %d", sdelay);
    DLOG("FeederConfig::init() days_off = %x", days_off);
    DLOG("FeederConfig::init() time_start = %d", time_start);
    DLOG("FeederConfig::init() time_stop = %d", time_stop);
    DLOG("FeederConfig::init() url = '%s'", url.c_str());
    DLOG("FeederConfig::init() parser = '%s'", parser.c_str());

	DLOG("FeederConfig::init() ic_port = %d", ic_port);
    DLOG("FeederConfig::init() log_level = %d", log_level);
    DLOG("FeederConfig::init() db_conninfo = '%s'", db_conninfo.c_str());
    DLOG("FeederConfig::init() cfg_fname = '%s'", cfg_fname.c_str());
    DLOG("FeederConfig::init() log_fname = '%s'", log_fname.c_str());
}

void FeederConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_feeder [options]\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d <seconds>    Delay between feeds in seconds\n");
    fprintf(stdout, "  -u <url>        Url of the source\n");
    fprintf(stdout, "  -p <parser>     Parser to use with the url\n");
}

