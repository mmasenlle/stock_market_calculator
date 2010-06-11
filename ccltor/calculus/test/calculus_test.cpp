
#include <stdio.h>
#include "utils.h"
#include "matrix.h"
#include "equation.h"

int main(int argc, char *argv[])
{
	double X[] = { 11, 12, 13, 21, 22, 23, 31, 32, 33 };
	double XT[9];
	
	matrix::transpose(3, X, XT);
	printf("\nX:  "); for (int i = 0; i < ARRAY_SIZE(X); i++) printf("%g ", X[i]);
	printf("\nXT: "); for (int i = 0; i < ARRAY_SIZE(XT); i++) printf("%g ", XT[i]);
	printf("\nX . XT: %g\n\n", matrix::dot(ARRAY_SIZE(X), X, XT));

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
}
