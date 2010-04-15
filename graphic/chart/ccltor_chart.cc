
#include "Chart.h"

int main(int argc, char *argv[])
{
	Chart::chart.config.init(argc, argv);
	Chart::chart.init();

	Chart::chart.show();
}
