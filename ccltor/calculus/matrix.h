#ifndef _MATRIX_H_
#define _MATRIX_H_

namespace matrix
{
	double dot(int n, const double *uu, const double *vv);

	double *transp(int n, const double *A, double *At);
	double *upper(int n, const double *A, double *U, double *uu = NULL);

	
	double det(int n, const double *M);
	void invert(int n, const double *M, double *M_1);
}

#endif
