
#include "CruncherConfig.h"
#include "CruncherManager.h"

int main(int argc, char *argv[])
{
	CruncherConfig::config.init(argc, argv);

	CruncherManager cruncher_manager;

	cruncher_manager.init();
	cruncher_manager.run();
}
