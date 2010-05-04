
#include <stdio.h>
#include "logger.h"
#include "Table.h"

Table::Table(const OutputConfig *cfg) : config(cfg)
{
	
}

void Table::output(const std::vector<int> t[2],
		const std::vector<std::vector<double> > *X)
{
	FILE *f = stdout;
	if (config->output_fname.length() && config->output_fname[0] != '-')
		f = fopen(config->output_fname.c_str(), "w");
	if (!f)
	{
		SELOG("Table::ouput() -> fopen(%s)", config->output_fname.c_str());
		return;
	}
	fprintf(f, "<table>\n<tr><th>DAY</th><th>TIME</th>");
	for (int j = 0; j < X->size(); j++)
		fprintf(f, "<th>VAL%d</th>", j + 1);
	fprintf(f, "</tr>\n");
	for (int i = 0; i < t[0].size(); i++)
	{
		fprintf(f, "<tr>");
		if (i < t[0].size()) fprintf(f, "<td>%08d</td>", t[0].at(i));
		else fprintf(f, "<td>&nbsp;</td>");
		if (i < t[1].size()) fprintf(f, "<td>%06d</td>", t[1].at(i));
		else fprintf(f, "<td>&nbsp;</td>");
		for (int j = 0; j < X->size(); j++)
			if (i < X->at(j).size()) fprintf(f, "<td>%f</td>", X->at(j).at(i));
			else fprintf(f, "<td>&nbsp;</td>");
		fprintf(f, "</tr>\n");
	}
	fprintf(f, "</table>\n\n");
	if (f != stdout)
		fclose(f);
}
