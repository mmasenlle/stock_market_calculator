
#include "Scheduler.h"

int main(int argc, char *argv[])
{
	Scheduler scheduler;
	scheduler.config.init(argc, argv);

	scheduler.init();
	scheduler.run();
}
