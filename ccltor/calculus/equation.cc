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

double equation::solve(int n, const double *yy, const double *A, double *xx)
{
	double error = DBL_MAX;
	double _xx[n];
	memcpy(_xx, xx, sizeof(_xx));
	for (int k = 0; k < 80; k++)
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
