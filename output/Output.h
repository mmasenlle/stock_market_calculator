#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "DBinterpolator.h"
#include "DBwealth.h"
#include "DBtrends.h"
#include "DBfeeder.h"
#include "DBstatistics.h"
#include "OutputConfig.h"
#include "OutputFront.h"

class Output
{
	double normax;
	OutputFront *front;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	DBtrends dbtrends;
	DBwealth dbwealth;
	DBinterpolator dbinterpolator;
	
	void get_data(const OutpDesc *desc, std::vector<double> *data, std::vector<int> times[2]);
	void merge_data(const std::vector<int> times[2], const std::vector<double> *data,
		std::vector<int> t[2], std::vector<std::vector<double> > *X);

public:
	OutputConfig config;
	
	Output();
	~Output();
	void init();
	void output();
};

#endif
