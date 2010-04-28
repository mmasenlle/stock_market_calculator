#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "DBfeeder.h"
#include "DBstatistics.h"
#include "OutputConfig.h"
#include "OutputFront.h"

class Output
{
	OutputFront *front;
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	
	void get_data(std::vector<double> *data, std::vector<int> *dates, std::vector<int> *times);

public:
	OutputConfig config;
	
	Output();
	~Output();
	void init();
	void output();
};

#endif
