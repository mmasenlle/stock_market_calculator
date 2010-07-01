#ifndef _EQUATION_H_
#define _EQUATION_H_

namespace equation
{
	double polyval(int n, const double *aa, double x);
	double *linsolve(int n, const double *bb, const double *L, const double *U, double *xx);
	double solve_gs(int n, const double *bb, const double *A, double *xx);
	double solve_cg(int n, const double *bb, const double *A, double *xx);
}

#endif
