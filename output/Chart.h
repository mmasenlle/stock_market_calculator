#ifndef _CHART_H_
#define _CHART_H_

#include "OutputConfig.h"
#include "OutputFront.h"

class Chart : public OutputFront
{
	const OutputConfig *config;
public:
	Chart(const OutputConfig *cfg);
	void output(const std::vector<int> t[2],
			const std::vector<std::vector<double> > *X);
};

#endif
