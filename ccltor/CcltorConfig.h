#ifndef _CCLTORCONFIG_H_
#define _CCLTORCONFIG_H_

#include <string>
#include "XPathConfig.h"

class CcltorConfig
{
protected:
	CcltorConfig();
    void init_pre(int argc, char *argv[]);
    void init_post(const XPathConfig &xpc, const char *key, int argc, char *argv[]);
    virtual void print_help() = 0;

public:
	int ic_port;
    int log_level;

	std::string db_conninfo;
    std::string cfg_fname;
    std::string log_fname;
};

#define FOR_OPT_ARG(_argc, _argv) \
    for (int _i = 1; _i < _argc; _i++) {\
        if(_argv[_i][0] == '-' && (_i + 1) < _argc && \
				(_argv[_i + 1][0] != '-' || !isalpha(_argv[_i + 1][1]))) {\
            const char *arg = _argv[_i + 1]; \
            switch(_argv[_i++][1])

#define FOR_OPT(_argc, _argv) \
    for (int _i = 1; _i < _argc; _i++) {\
        if(_argv[_i][0] == '-') {\
            switch(_argv[_i][1])

#define END_OPT }}

#endif

