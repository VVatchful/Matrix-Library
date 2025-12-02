#ifndef MATRIX_H
#define MATRIX_H

typedef struct Matrix {
    int rows;
    int cols;
    int **data;
} Matrix;

Matrix* create_matrix(int r, int c);
void dealloc_matrix(Matrix *m);
void init_zero(Matrix *m);
void init_random(Matrix *m, int min_value, int max_value);
void matrix_print(Matrix *m);
Matrix* dupe_matrix(Matrix *m);

Matrix* addition(Matrix *A, Matrix *B);
Matrix* scalar_multiply(Matrix *m, int scalar);
Matrix* transpose(Matrix *m);
Matrix* multiply_matrix(Matrix *A, Matrix *B);

double determinant(Matrix *m);
Matrix* matrix_inverse(Matrix *m);

void test_addition(void);
void test_multiply(void);
void test_determinant(void);
void test_inverse(void);

#endif