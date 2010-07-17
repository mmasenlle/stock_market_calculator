
#include <stdio.h>
#include "utils.h"
#include "matrix.h"
#include "equation.h"

int main(int argc, char *argv[])
{
	double Xr[] = { 11, 12, 13, 21, 22, 23 };
	double XrT[6], XrXrT[4], XrTXr[9];
	matrix::transp(2, 3, Xr, XrT);
	printf("\nXr:  "); for (int i = 0; i < ARRAY_SIZE(Xr); i++) printf("%g ", Xr[i]);
	printf("\nXrT: "); for (int i = 0; i < ARRAY_SIZE(XrT); i++) printf("%g ", XrT[i]);
	matrix::mul(3, 2, 3, XrT, Xr, XrTXr);
	printf("\nXrTXr:  "); for (int i = 0; i < ARRAY_SIZE(XrTXr); i++) printf("%g ", XrTXr[i]);
	matrix::mul(2, 3, 2, Xr, XrT, XrXrT);
	printf("\nXrXrT:  "); for (int i = 0; i < ARRAY_SIZE(XrXrT); i++) printf("%g ", XrXrT[i]);

	
	double X[] = { 11, 12, 13, 21, 22, 23, 31, 32, 33 };
	double C[9],XT[9];
	
	matrix::transp(3, 3, X, XT);
	printf("\nX:  "); for (int i = 0; i < ARRAY_SIZE(X); i++) printf("%g ", X[i]);
	printf("\nXT: "); for (int i = 0; i < ARRAY_SIZE(XT); i++) printf("%g ", XT[i]);
	printf("\nX . XT: %g", matrix::dot(ARRAY_SIZE(X), X, XT));
	matrix::mul(3, 3, 3, X, XT, C);
	printf("\nX * XT:  "); for (int i = 0; i < ARRAY_SIZE(C); i++) printf("%g ", C[i]);

	double A[] = { 1, 2, 3, 4, 5, 7, 6, 8, 9 };
//	double A[] = { 1, -1, 0, -1, 1, -1, 0, -1, 1 };
	printf("\nA:  "); for (int i = 0; i < ARRAY_SIZE(A); i++) printf("%g ", A[i]);
	double L[9],U[9];
	matrix::mul(3, 3, 3, A, A, C);
	printf("\nA * A:  "); for (int i = 0; i < ARRAY_SIZE(C); i++) printf("%g ", C[i]);
	printf("\ndet(A): %g; det(X): %g; det(XT): %g; det(A*A): %g",
	    matrix::det(3, A), matrix::det(3, X), matrix::det(3, XT), matrix::det(3, C));
	matrix::lu(3, A, L, U);
	printf("\nL:  "); for (int i = 0; i < ARRAY_SIZE(L); i++) printf("%g ", L[i]);
	printf("\nU:  "); for (int i = 0; i < ARRAY_SIZE(U); i++) printf("%g ", U[i]);
	matrix::mul(3, 3, 3, L, U, C);
	printf("\nLU:  "); for (int i = 0; i < ARRAY_SIZE(C); i++) printf("%g ", C[i]);

	double xx[3], bb[3] = { 6, 16, 23 };
	equation::linsolve(3, bb, L, U, xx);
	printf("\nxx:  "); for (int i = 0; i < ARRAY_SIZE(xx); i++) printf("%g ", xx[i]);
	double cc[3] = { 4, 8, 11 };
	equation::linsolve(3, cc, L, U, xx);
	printf("\nxx:  "); for (int i = 0; i < ARRAY_SIZE(xx); i++) printf("%g ", xx[i]);
#if 0	
	matrix::lu(3, X, L, U);
	printf("\nL:  "); for (int i = 0; i < ARRAY_SIZE(L); i++) printf("%g ", L[i]);
	printf("\nU:  "); for (int i = 0; i < ARRAY_SIZE(U); i++) printf("%g ", U[i]);
	matrix::mul(3, L, U, C);
	printf("\nX->LU->X:  "); for (int i = 0; i < ARRAY_SIZE(C); i++) printf("%g ", C[i]);
	printf("\n\n");

	double f1[] = { 1, 1 };
	double f2[] = { 0, 0, 1 };
	printf("f1: y = 1 + x\nf2: y = x2\n\n");
	printf("f1(0): %g f2(0): %g\n", equation::polyval(ARRAY_SIZE(f1), f1, 0),
								equation::polyval(ARRAY_SIZE(f2), f2, 0));
	printf("f1(1): %g f2(1): %g\n", equation::polyval(ARRAY_SIZE(f1), f1, 1),
								equation::polyval(ARRAY_SIZE(f2), f2, 1));
	printf("f1(2): %g f2(2): %g\n", equation::polyval(ARRAY_SIZE(f1), f1, 2),
								equation::polyval(ARRAY_SIZE(f2), f2, 2));
	printf("f1(-1): %g f2(-1): %g\n", equation::polyval(ARRAY_SIZE(f1), f1, -1),
								equation::polyval(ARRAY_SIZE(f2), f2, -1));
	printf("X(-1.1): %g\n", equation::polyval(ARRAY_SIZE(X), X, -1.1));

	double yy[] = { 10, 20, 30 };
	double xx[3] = { 0.1, -0.1, 0.1 };
	double error = equation::solve(ARRAY_SIZE(yy), yy, X, xx);
	printf("\nSolve (y: 10, 20, 30; X): %g %g %g error: %g (%g %g %g)\n\n",
	    xx[0], xx[1], xx[2], error, matrix::dot(ARRAY_SIZE(xx), xx, X),
	    matrix::dot(ARRAY_SIZE(xx), xx, &X[3]), matrix::dot(ARRAY_SIZE(xx), xx, &X[6]));
#endif
}
