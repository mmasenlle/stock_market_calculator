
#include "Output.h"

int main(int argc, char *argv[])
{
	Output output;
	output.config.init(argc, argv);

	output.init();
	output.output();
}
