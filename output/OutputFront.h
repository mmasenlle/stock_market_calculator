#ifndef _OUPUTMODE_H_
#define _OUPUTMODE_H_

#include <vector>

struct OutputFront
{
	virtual void output(const std::vector<int> t[2],
			const std::vector<std::vector<double> > *X) = 0;
	virtual ~OutputFront() {};
};

#endif
