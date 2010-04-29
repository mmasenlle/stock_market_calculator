
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
	if (config->type == OUTPTYPE_ALL)
	{
		int d = dates->size() ? dates->front() : 0;
		int t = times->size() ? times->front() : 0;
		for (int i = 0, j = 0; i < data->size(); i++)
		{
			if (d == dates->at(i))
			{
				int dt = times->at(i) - t;
				int h = dt / 10000;
				dt -= (h * 10000);
				int m = dt / 100;
				dt -= (m * 100);
				j += (((h * 60) + m) * 60) + dt;
			}
			d = dates->at(i);
			t = times->at(i);
			fprintf(f, "%d\t%f\n", j, data->at(i));
		}
	}
	else
	{
		for (int i = 0; i < data->size(); i++)
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
	if (config->output_mode == OUTPMODE_PNG)
	{
		fprintf(f, "set term png\n");
		if (config->output_fname.length() && config->output_fname[0] != '-')
			fprintf(f, "set output '%s'\n", config->output_fname.c_str());
	}
	fprintf(f, "set title '%s'\n", config->value.c_str());
	if (config->type == OUTPTYPE_ALL && dates->size() && times->size())
	{
		fprintf(f, "set xlabel 'from %08d %06d to %08d %06d'\n",
				dates->front(), times->front(), dates->back(), times->back());
	}
	else if(dates->size())
	{
		fprintf(f, "set xlabel 'from %08d to %08d'\n", dates->front(), dates->back());
	}
	else
	{
		fprintf(f, "set xlabel 'from %08d %06d to %08d %06d'\n",
				config->day_start, config->time_start, config->day_end, config->time_end);
	}
	fprintf(f, "set ylabel '%s of %s'\n", config->getType(), config->getItem());
	fprintf(f, "set style data lines%s\nplot '%s'\n", data->size() < 50 ? "points" : "", datafile);
	fclose(f);
	
	for (int fd = 3; fd < 32; fd++) close(fd);
	execlp("gnuplot", "gnuplot", "-persist", plotfile, NULL);
}
