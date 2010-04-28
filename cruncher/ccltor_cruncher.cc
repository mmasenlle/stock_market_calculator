
#include "CruncherConfig.h"
#include "CruncherManager.h"

int main(int argc, char *argv[])
{
	CruncherManager cruncher_manager;
	
	cruncher_manager.ccfg->init(argc, argv);

	cruncher_manager.init();
	cruncher_manager.run();
}
