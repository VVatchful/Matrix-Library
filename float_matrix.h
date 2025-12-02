#ifndef FLOAT_MATRIX_H
#define FLOAT_MATRIX_H

#include "matrix.h"

typedef struct FloatMatrix {
    int rows;
    int cols;
    double **data;
} FloatMatrix;

FloatMatrix* create_float_matrix(int r, int c);
void dealloc_float_matrix(FloatMatrix *m);
void float_matrix_print(FloatMatrix *m);
void init_float_zero(FloatMatrix *m);

FloatMatrix* int_matrix_to_float(Matrix *m);
Matrix* float_matrix_to_int(FloatMatrix *m);
double float_determinant(FloatMatrix *m);
FloatMatrix* float_matrix_inverse(FloatMatrix *m);
FloatMatrix* float_multiply_matrix(FloatMatrix *A, FloatMatrix *B);

void test_float_inverse(void);
void test_float_determinant(void);

#endif