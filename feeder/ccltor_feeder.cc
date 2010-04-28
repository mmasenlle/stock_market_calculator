
#include "Feeder.h"

int main(int argc, char *argv[])
{
	Feeder::feeder.config.init(argc, argv);
	
	Feeder::feeder.init();
	
	for (;;) Feeder::feeder.run();
}
