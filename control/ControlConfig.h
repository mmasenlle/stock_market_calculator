#ifndef _CONTROLCONFIG_H_
#define _CONTROLCONFIG_H_

#include <vector>
#include "ICPeer.h"
#include "CcltorConfig.h"

class ControlConfig : public CcltorConfig
{
    void print_help();

	bool strto_event(const char *str, char **p);
	bool strto_subevent(const char *str);
    void get_event(const char *ev_str);
    void add_peer(const char *p_str);
    void add_peers(const char *peers);

public:
	int event, sub_event;
	std::vector<ICPeer> peers;

    ControlConfig();
    void init(int argc, char *argv[]);
};

#endif
