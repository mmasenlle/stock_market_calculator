#ifndef _OUPUTMODE_H_
#define _OUPUTMODE_H_

#include <vector>

struct OutputFront
{
	virtual void output(const std::vector<double> *data,
			const std::vector<int> *dates, const std::vector<int> *times) = 0;
	virtual ~OutputFront() {};
};

#endif
