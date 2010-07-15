#ifndef _MATRIX_H_
#define _MATRIX_H_

namespace matrix
{
	double dot(int n, const double *uu, const double *vv);
	double *mul(int m, int n, int p, const double *A, const double *B, double *C);
	double *transp(int m, int n, const double *A, double *At);
	double *lu(int n, const double *A, double *L, double *U);
	double det(int n, const double *A);

//	void invert(int n, const double *M, double *M_1);
}

#endif
