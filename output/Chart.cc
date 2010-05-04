
#include <stdio.h>
#include "logger.h"
#include "Chart.h"

Chart::Chart(const OutputConfig *cfg) : config(cfg)
{
	
}

void Chart::output(const std::vector<int> t[2],
		const std::vector<std::vector<double> > *X)
{
	if (X->size() && X->front().size())
	{
		char datafile[128], plotfile[128];
		snprintf(datafile, sizeof(datafile), "%s/ccltorplot_%d.dat", config->tmp_path.c_str(), getpid());
		FILE *f = fopen(datafile, "w");
		if (!f)
		{
			SELOG("Chart::ouput() -> fopen(%s)", datafile);
			return;
		}
		if (t[1].size())
		{
			int d = t[0].size() ? t[0].front() : 0;
			int s = t[1].size() ? t[1].front() : 0;
			for (int i = 0, j = 0; i < t[0].size(); i++)
			{
				if (d == t[0].at(i))
				{
					int dt = t[1].at(i) - s;
					int h = dt / 10000;
					dt -= (h * 10000);
					int m = dt / 100;
					dt -= (m * 100);
					j += (((h * 60) + m) * 60) + dt;
				}
				else j++;
				d = t[0].at(i);
				s = t[1].at(i);
				fprintf(f, "%d", j);
				for (int k = 0; k < X->size(); k++)
					fprintf(f, "\t%f", X->at(k).at(i));
				fprintf(f, "\n");
			}
		}
		else
		{
			for (int i = 0; i < t[0].size(); i++)
			{
				fprintf(f, "%d", i);
				for (int j = 0; j < X->size(); j++)
					fprintf(f, "\t%f", X->at(j).at(i));
				fprintf(f, "\n");
			}
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
		fprintf(f, "set title '%s'\n", config->outpdesc.value.c_str());
		if (t[1].size())
		{
			fprintf(f, "set xlabel 'from %08d %06d to %08d %06d'\n",
			    t[0].front(), t[1].front(), t[0].back(), t[1].back());
		}
		else if(t[0].size())
		{
			fprintf(f, "set xlabel 'from %08d to %08d'\n", t[0].front(), t[0].back());
		}
		fprintf(f, "set ylabel '%s of %s'\n", config->getType(), config->getItem());
		fprintf(f, "set style data lines%s\n", t[0].size() < 50 ? "points" : "");
		fprintf(f, "plot '%s' using 1:2", datafile);
		for (int j = 1; j < X->size(); j++)
			fprintf(f, ", '%s' using 1:%d", datafile, j + 2);
		fprintf(f, "\n");
		fclose(f);

		for (int fd = 3; fd < 16; fd++) close(fd);
		execlp("gnuplot", "gnuplot", "-persist", plotfile, NULL);
	}
}
