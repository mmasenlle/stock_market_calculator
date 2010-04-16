
#include <stdlib.h>
#include "logger.h"
#include "CcltorConfig.h"

#define CCLTOR_CONF_FNAME "ccltor.xml"

#define IC_PORT_KEY		"/ic_port"
#define DB_CONNINFO_KEY	"/db_conninfo"
#define LOG_FNAME_KEY 	"/log_fname"
#define LOG_LEVEL_KEY 	"/log_level"

#define CCLTOR_DB_CONNINFO_KEY "/ccltor" DB_CONNINFO_KEY
#define CCLTOR_LOG_LEVEL_KEY "/ccltor" LOG_LEVEL_KEY

#define CCLTOR_LOG_LEVEL_ENV "CCLTOR_LOG_LEVEL"

CcltorConfig::CcltorConfig()
{
	ic_port = 17000;
	log_level = 2;
	db_conninfo = "user=ccltor dbname=calculinator";
}

void CcltorConfig::init_pre(int argc, char *argv[])
{
    cfg_fname = argv[0]; cfg_fname += ".xml";
    log_fname = argv[0]; log_fname += ".log";

    std::string xp_value;
    XPathConfig xpconf(CCLTOR_CONF_FNAME);
	xpconf.getValue(CCLTOR_DB_CONNINFO_KEY, &db_conninfo);
    if (xpconf.getValue(CCLTOR_LOG_LEVEL_KEY, &xp_value))
        log_level = atoi(xp_value.c_str());

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
        print_help();
        fprintf(stdout, "  -c <conninfo>   DB connection data\n");
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
    case 'f': cfg_fname = arg; break;
    }
    END_OPT;
}

void CcltorConfig::init_post(const XPathConfig &xpc, const char *key, int argc, char *argv[])
{
    std::string xp_value, skey = key;
	if (xpc.getValue((skey + IC_PORT_KEY).c_str(), &xp_value))
        ic_port = atoi(xp_value.c_str());
    if (xpc.getValue((skey + LOG_LEVEL_KEY).c_str(), &xp_value))
        log_level = atoi(xp_value.c_str());
	xpc.getValue((skey + DB_CONNINFO_KEY).c_str(), &db_conninfo);
    xpc.getValue((skey + LOG_FNAME_KEY).c_str(), &log_fname);

    if (getenv(CCLTOR_LOG_LEVEL_ENV))
        log_level = atoi(getenv(CCLTOR_LOG_LEVEL_ENV));

    FOR_OPT_ARG(argc, argv)
    {
    case 'c': db_conninfo = arg; break;
    case 'l': log_fname = arg; break;
    }
    END_OPT;

    if (log_level >= 0 && log_fname.length())
    {
        INIT_DEFAULT_LOGGER(
            log_fname[0] == '_' ? NULL : log_fname.c_str(),
            log_level);
    }
}

