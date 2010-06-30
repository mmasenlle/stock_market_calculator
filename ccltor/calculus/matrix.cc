
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

double *matrix::mul(int n, const double *A, const double *B, double *C)
{
	memset(C, 0, n * n * sizeof(*C));
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++)
				C[(i * n) + j] += (A[(i * n) + k] * B[(k * n) + j]);
	return C;
}

double *matrix::transp(int n, const double *A, double *At)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			At[(j * n) + i] = A[(i * n) + j];
	return At;
}

double *matrix::lu(int n, const double *A, double *L, double *U)
{
	memset(L, 0, n * n * sizeof(*L));
	memcpy(U, A, n * n * sizeof(*U));
	for (int p = 0; p < n; p++)
	{
		if (utils::equald(U[(p * n) + p], 0.0))
			return NULL;
		L[(p * n) + p] = 1.0;
		for (int i = p + 1; i < n; i++)
		{
			if (!utils::equald(U[(i * n) + p], 0.0))
			{
				L[(i * n) + p] = U[(i * n) + p] / U[(p * n) + p];
				U[(i * n) + p] = 0.0;
				for (int j = p + 1; j < n; j++)
					U[(i * n) + j] -= (L[(i * n) + p] * U[(p * n) + j]);
			}
		}
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
