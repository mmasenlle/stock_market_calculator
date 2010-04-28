
#include <stdio.h>
#include "logger.h"
#include "Chart.h"

Chart::Chart(const OutputConfig *cfg) : config(cfg)
{
	
}

void Chart::output(const std::vector<double> *data,
		const std::vector<int> *dates, const std::vector<int> *times)
{
	char datafile[128], plotfile[128];
	snprintf(datafile, sizeof(datafile), "%s/ccltorplot_%d.dat", config->tmp_path.c_str(), getpid());
	FILE *f = fopen(datafile, "w");
	if (!f)
	{
		SELOG("Chart::ouput() -> fopen(%s)", datafile);
		return;
	}
	for (int i = 0; i < data->size(); i++)
	{	//FIXME: put also date and time values
		fprintf(f, "%d\t%f\n", i, data->at(i));
	}
	fclose(f);
	snprintf(plotfile, sizeof(plotfile), "%s/ccltorplot_%d.plot", config->tmp_path.c_str(), getpid());
	f = fopen(plotfile, "w");
	if (!f)
	{
		SELOG("Chart::ouput() -> fopen(%s)", plotfile);
		return;
	}
	fprintf(f, "set title '%s'\n", config->value.c_str());
	fprintf(f, "set xlabel 'from %08d %06d to %08d %06d'\n",
			config->day_start, config->time_start, config->day_end, config->time_end);
	fprintf(f, "set ylabel '%s of %s'\n", config->getType(), config->getItem());
	fprintf(f, "set style data linespoints\nplot '%s'\n", datafile);
	fclose(f);
	
	for (int fd = 3; fd < 32; fd++) close(fd);
	execlp("gnuplot", "gnuplot", "-persist", plotfile, NULL);
}
