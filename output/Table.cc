
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
	if (config->output_fname.length() && config->output_fname[0] != '-')
		f = fopen(config->output_fname.c_str(), "w");
	if (!f)
	{
		SELOG("Table::ouput() -> fopen(%s)", config->output_fname.c_str());
		return;
	}
	fprintf(f, "<h2>%s</h2>\n", config->value.c_str());
	fprintf(f, "<h3>%s of %s</h3>\n", config->getType(), config->getItem());
	fprintf(f, "<table>\n<tr><th>DAY</th><th>TIME</th><th>VALUE</th></tr>\n");
	for (int i = 0; i < data->size(); i++)
	{
		fprintf(f, "<tr>");
		if (dates && i < dates->size()) fprintf(f, "<td>%08d</td>", dates->at(i));
		else fprintf(f, "<td>&nbsp;</td>");
		if (times && i < times->size()) fprintf(f, "<td>%06d</td>", times->at(i));
		else fprintf(f, "<td>&nbsp;</td>");
		fprintf(f, "<td>%f</td></tr>\n", data->at(i));
	}
	fprintf(f, "</table>\n\n");
	if (f != stdout)
		fclose(f);
}
