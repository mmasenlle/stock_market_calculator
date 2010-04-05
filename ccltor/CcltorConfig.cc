
#include <stdlib.h>
#include "logger.h"
#include "CcltorConfig.h"

#define CCLTOR_CONF_FNAME "ccltor.xml"

#define LOG_LEVEL_KEY "/log_level"
#define LOG_FNAME_KEY "/log_fname"
#define CCLTOR_LOG_LEVEL_KEY "/ccltor" LOG_LEVEL_KEY

#define CCLTOR_LOG_LEVEL_ENV "CCLTOR_LOG_LEVEL"


void CcltorConfig::init_pre(CcltorConfig *cfg, int argc, char *argv[])
{
    cfg->log_level = 2;
    cfg->cfg_fname = argv[0]; cfg->cfg_fname += ".xml";
    cfg->log_fname = argv[0]; cfg->log_fname += ".log";

    std::string xp_value;
    XPathConfig xpconf(CCLTOR_CONF_FNAME);
    if (xpconf.getValue(CCLTOR_LOG_LEVEL_KEY, &xp_value))
        cfg->log_level = atoi(xp_value.c_str());

    if (getenv(CCLTOR_LOG_LEVEL_ENV))
        cfg->log_level = atoi(getenv(CCLTOR_LOG_LEVEL_ENV));

    FOR_OPT(argc, argv)
    {
    case 'v':
        fprintf(stdout, "%s\nCalculinator Suite v" CCLTOR_VERSION "\n"
		"Rev " CCLTOR_REVISION "\nBuilt " __DATE__ " (" __TIME__ ")"
#ifndef NDEBUG
		" debug\n\n", argv[0]);
#else
		"\n\n", argv[0]);
#endif
	fflush(stdout);
        exit(0);
    case 'h':
        cfg->print_help();
        fprintf(stdout, "  -f <file>       Configuration file\n");
        fprintf(stdout, "  -l <file>       Logger file\n");
        fprintf(stdout, "  -v              Print version and exit\n");
        fprintf(stdout, "  -h              Print this help and exit\n\n");
        fflush(stdout);
        exit(0);
    }
    END_OPT;
    FOR_OPT_ARG(argc, argv)
    {
    case 'f': cfg->cfg_fname = arg; break;
    }
    END_OPT;
}

void CcltorConfig::init_post(CcltorConfig *cfg, const XPathConfig &xpc,
    const char *key, int argc, char *argv[])
{
    std::string xp_value, skey = key;
    if (xpc.getValue((skey + LOG_LEVEL_KEY).c_str(), &xp_value))
        cfg->log_level = atoi(xp_value.c_str());
    xpc.getValue((skey + LOG_FNAME_KEY).c_str(), &cfg->log_fname);

    FOR_OPT_ARG(argc, argv)
    {
    case 'l': cfg->log_fname = arg; break;
    }
    END_OPT;

    if (cfg->log_level >= 0 && cfg->log_fname.length())
    {
        INIT_DEFAULT_LOGGER(
            cfg->log_fname[0] == '_' ? NULL : cfg->log_fname.c_str(),
            cfg->log_level);
    }
}

