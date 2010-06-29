
#include <string.h>
#include "utils.h"
#include "matrix.h"

double matrix::dot(int n, const double *uu, const double *vv)
{
	double y = 0;
	for (int j = 0; j < n; j++)
		y += uu[j] * vv[j];
	return y;
}

double *matrix::transp(int n, const double *A, double *At)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			At[(j * n) + i] = A[(i * n) + j];
	return At;
}

double *matrix::upper(int n, const double *A, double *U, double *uu)
{
	memcpy(U, A, n * sizeof(*U));
	for (int p = 0; p < n; p++)
		for (int i = p + 1; i < n; i++)
	{
		if (utils::equald(A[(p * n) + p], 0.0))
			return NULL;
		double r = A[(i * n) + p] / A[(p * n) + p];
		if (uu) *uu++ = r;
		for (int j = 0; j < n; j++)
			U[(i * n) + j] = (j <= p) ? 0.0 : A[(i * n) + j] - (r * A[(p * n) + j]);
	}
	return U;
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
