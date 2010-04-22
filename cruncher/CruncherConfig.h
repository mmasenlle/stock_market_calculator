#ifndef _CRUNCHERCONFIG_H_
#define _CRUNCHERCONFIG_H_

#include <vector>
#include <string>
#include "ICPeer.h"
#include "CcltorConfig.h"

class CruncherConfig : public CcltorConfig
{
    void print_help();

	void add_plugins(const char *ps);
	void add_feeder(const char *p_str);
    
public:
	std::string plugins_path;
	std::vector<std::string> plugins;
	
	std::vector<ICPeer> feeders;

	CruncherConfig();
    void init(int argc, char *argv[]);
};

#endif
