#include <float.h>
#include <math.h>
#include <string.h>
#include "logger.h"
#include "matrix.h"
#include "equation.h"


double equation::polyval(int n, const double *aa, double x)
{// y = a0 + a1*x + a2*x2 + .. + an-1*xn-1
	double y = aa[n - 1];
	for (int j = 2; j <= n; j++)
		y = aa[n - j] + (x * y);
	return y;
}

double *equation::linsolve(int n, const double *bb, const double *L, const double *U, double *xx)
{
	double _bb[n];
	memcpy(_bb, bb, sizeof(_bb));
	for (int j = 0; j < n; j++)
		for (int i = j + 1; i < n; i++)
			_bb[i] -= (L[(i*n)+j] * _bb[j]);
	for (int i = n - 1; i >= 0; i--)
	{
		xx[i] = _bb[i];
		for (int j = n - 1; j > i; j--)
			xx[i] -= (U[(i*n)+j] * xx[j]);
		xx[i] /= U[(i*n)+i];
	}
	return xx;
}

double equation::solve_gs(int n, const double *yy, const double *A, double *xx)
{
	double error = DBL_MAX;
	double _xx[n];
	memcpy(_xx, xx, sizeof(_xx));
	for (int k = 0; k < 800000; k++)
	{
		double e = 0;
		for (int i = 0; i < n; i++)
			e += fabs(yy[i] - matrix::dot(n, &A[i*n], _xx));
if (k < 2) DLOG("equation::solve() -> k: %d, e: %g error: %g\n", k, e, error);
		if (e < error)
		{
			error = e;
			memcpy(xx, _xx, sizeof(_xx));
			k = 0;
		}
		for (int i = 0; i < n; i++)
		{// Gauss-Seidel method
			double sigma = 0;
			for (int j = 0; j < n; j++)
				if (j != i) sigma += A[(i*n)+j] * _xx[j];
			_xx[i] = (yy[i] - sigma) / A[(i*n)+i];
		}
	}
	return error;
}

double equation::solve_cg(int n, const double *bb, const double *A, double *xx)
{
	double error = DBL_MAX;

	return error;
}
