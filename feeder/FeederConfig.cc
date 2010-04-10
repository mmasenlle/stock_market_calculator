
#include "logger.h"
#include "FeederConfig.h"

FeederConfig FeederConfig::feederConfig;

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
    init_pre(&feederConfig, argc, argv);

    std::string xp_value, key = "/feeder";
    XPathConfig xpconf(feederConfig.cfg_fname.c_str());
    if (xpconf.getValue((key + "/sdelay").c_str(), &xp_value))
        feederConfig.sdelay = atoi(xp_value.c_str());
    if (xpconf.getValue((key + "/days_off").c_str(), &xp_value))
    {
        feederConfig.days_off = 0;
        for (int i = 0; i < xp_value.length() && i < 7; i++)
            if (xp_value[i] == '1') feederConfig.days_off |= (1 << i);
    }
    if (xpconf.getValue((key + "/time_start").c_str(), &xp_value))
    {
        int h, m;
        if (sscanf(xp_value.c_str(), "%d:%d", &h, &m) == 2)
            feederConfig.time_start = (h * 60) + m;
    }
    if (xpconf.getValue((key + "/time_stop").c_str(), &xp_value))
    {
        int h, m;
        if (sscanf(xp_value.c_str(), "%d:%d", &h, &m) == 2)
            feederConfig.time_stop = (h * 60) + m;
    }
    xpconf.getValue((key + "/url").c_str(), &feederConfig.url);
    xpconf.getValue((key + "/parser").c_str(), &feederConfig.parser);

    FOR_OPT_ARG(argc, argv)
    {
    case 'd': feederConfig.sdelay = atoi(arg); break;
    case 'u': feederConfig.url = arg; break;
    case 'p': feederConfig.parser = arg; break;
    }
    END_OPT;

    init_post(&feederConfig, xpconf, key.c_str(), argc, argv);

    DLOG("FeederConfig::init() sdelay = %d", feederConfig.sdelay);
    DLOG("FeederConfig::init() days_off = %x", feederConfig.days_off);
    DLOG("FeederConfig::init() time_start = %d", feederConfig.time_start);
    DLOG("FeederConfig::init() time_stop = %d", feederConfig.time_stop);
    DLOG("FeederConfig::init() url = '%s'", feederConfig.url.c_str());
    DLOG("FeederConfig::init() parser = '%s'", feederConfig.parser.c_str());
	DLOG("FeederConfig::init() ic_port = %d", feederConfig.ic_port);
    DLOG("FeederConfig::init() log_level = %d", feederConfig.log_level);
    DLOG("FeederConfig::init() cfg_fname = '%s'", feederConfig.cfg_fname.c_str());
    DLOG("FeederConfig::init() log_fname = '%s'", feederConfig.log_fname.c_str());
}

void FeederConfig::print_help()
{
    fprintf(stdout, "Usage: ccltor_feeder [options]\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d <seconds>    Delay between feeds in seconds\n");
    fprintf(stdout, "  -u <url>        Url of the source\n");
    fprintf(stdout, "  -p <parser>     Parser to use with the url\n");
}

