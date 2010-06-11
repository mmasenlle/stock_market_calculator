#ifndef _EQUATION_H_
#define _EQUATION_H_

namespace equation
{
	double polyval(int n, const double *aa, double x);
	double solve(int n, const double *yy, const double *A, double *xx);
}

#endif
