
#include <stdio.h>
#include "logger.h"
#include "Table.h"

Table::Table(const OutputConfig *cfg) : config(cfg)
{
	
}

void Table::output(const std::vector<double> *data,
		const std::vector<int> *dates, const std::vector<int> *times)
{
	FILE *f = stdout;
	if (config->output_fname.length() && config->output_fname[0] != '_')
		f = fopen(config->output_fname.c_str(), "w");
	if (!f)
	{
		SELOG("Table::ouput() -> fopen(%s)", config->output_fname.c_str());
		return;
	}
	fprintf(f, "<h1>%s</h1>\n", config->value.c_str());
	fprintf(f, "<h2>%s of %s</h2>\n", config->getType(), config->getItem());
	fprintf(f, "<h3>from %08d %06d to %08d %06d</h3>\n",
			config->day_start, config->time_start, config->day_end, config->time_end);
	fprintf(f, "<table>\n<tr><td>DATA</td><td>DATE</td><td>TIME</td></tr>\n");
	for (int i = 0; i < data->size(); i++)
	{
		fprintf(f, "<tr><td>%f</td>", data->at(i));
		if (dates && i < dates->size()) fprintf(f, "<td>%08d</td>", dates->at(i));
		if (times && i < times->size()) fprintf(f, "<td>%06d</td>", times->at(i));
		fprintf(f, "</tr>\n", data->at(i));
	}
	fprintf(f, "</table>\n\n");
	if (f != stdout)
		fclose(f);
}
