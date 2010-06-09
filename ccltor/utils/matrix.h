#ifndef _MATRIX_H_
#define _MATRIX_H_

namespace matrix
{
	void transpose(int n, const double *M, double *MT);
	double det(int n, const double *M);
	void invert(int n, const double *M, double *M_1);
}

#endif
