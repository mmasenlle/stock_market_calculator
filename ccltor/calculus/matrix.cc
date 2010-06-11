
#include <string.h>
#include "utils.h"
#include "matrix.h"

double matrix::dot(int n, const double *u, const double *v)
{
	double y = 0;
	for (int j = 0; j < n; j++)
		y += u[j] * v[j];
	return y;
}

void matrix::transpose(int n, const double *M, double *MT)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			MT[(j * n) + i] = M[(i * n) + j];
}

//FIXME: below here very inefficient
static double matrix_cofactor(int n, const double *M, int i, int j)
{
	double subM[n - 1][n - 1];
	for (int k = 0; k < (n - 1); k++)
		for (int l = 0; l < (n - 1); l++)
			subM[k][l] = M[(k+(k>=i))*n + (l+(l>=j))];
	return (i+j)%2 ? -matrix::det(n-1, (double *)subM) : matrix::det(n-1, (double *)subM);
}

double matrix::det(int n, const double *M)
{
	if(n == 1) return M[0];
	if(n == 2) return ((M[0] * M[3]) - (M[1] * M[2]));
	double d = 0;
	for (int i = 0; i < n; i++)
		d += (M[i * n] * matrix_cofactor(n, M, i, 0));
	return d;
}

void matrix::invert(int n, const double *M, double *M_1)
{
	double detM = det(n, M);
	if (utils::equald(detM, 0.0))
	{
		memset(M_1, 0, n * n * sizeof(double));
	}
	else
	{
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				M_1[(j * n) + i] = matrix_cofactor(n, M, i, j) / detM;
	}
}
