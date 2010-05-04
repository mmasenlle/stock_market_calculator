#ifndef _TABLE_H_
#define _TABLE_H_

#include "OutputConfig.h"
#include "OutputFront.h"

class Table : public OutputFront
{
	const OutputConfig *config;
public:
	Table(const OutputConfig *cfg);
	void output(const std::vector<int> t[2],
			const std::vector<std::vector<double> > *X);
};

#endif
