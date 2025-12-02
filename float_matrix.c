#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "float_matrix.h"

double determinant(Matrix *m);

FloatMatrix* create_float_matrix(int r, int c) {
    FloatMatrix *m = (FloatMatrix *)malloc(sizeof(FloatMatrix));
    if (m == NULL) {
        perror("Failed to allocate memory for FloatMatrix");
        return NULL;
    }

    m->rows = r;
    m->cols = c;

    m->data = (double **)malloc(r * sizeof(double *));
    if (m->data == NULL) {
        perror("Failed to allocate memory for data rows");
        free(m);
        return NULL;
    }

    for (int i = 0; i < r; i++) {
        m->data[i] = (double *)malloc(c * sizeof(double));
        if (m->data[i] == NULL) {
            perror("Failed to allocate memory for data columns");
            for (int j = 0; j < i; j++) {
                free(m->data[j]);
            }
            free(m->data);
            free(m);
            return NULL;
        }
    }
    return m;
}

void dealloc_float_matrix(FloatMatrix *m) {
    if (m == NULL) return;
    for (int i = 0; i < m->rows; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

void init_float_zero(FloatMatrix *m) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            m->data[i][j] = 0.0;
        }
    }
}

void float_matrix_print(FloatMatrix *m) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            printf("%8.4f ", m->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


FloatMatrix* int_matrix_to_float(Matrix *m) {
    if (m == NULL) return NULL;

    FloatMatrix *fm = create_float_matrix(m->rows, m->cols);
    if (fm == NULL) return NULL;

    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            fm->data[i][j] = (double)m->data[i][j];
        }
    }

    return fm;
}

Matrix* float_matrix_to_int(FloatMatrix *m) {
    if (m == NULL) return NULL;

    extern Matrix* create_matrix(int r, int c);

    Matrix *im = create_matrix(m->rows, m->cols);
    if (im == NULL) return NULL;

    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            im->data[i][j] = (int)round(m->data[i][j]);
        }
    }

    return im;
}

double float_determinant(FloatMatrix *m) {
    if (m->rows != m->cols) {
        printf("Determinant undefined for non-square matrices\n");
        return 0.0;
    }

    int n = m->rows;

    if (n == 1) {
        return m->data[0][0];
    }

    if (n == 2) {
        return (m->data[0][0] * m->data[1][1]) - (m->data[0][1] * m->data[1][0]);
    }
    double det = 0.0;

    for (int col = 0; col < n; col++) {
        FloatMatrix *submatrix = create_float_matrix(n - 1, n - 1);
        if (submatrix == NULL) {
            return 0.0;
        }

        for (int i = 1; i < n; i++) {
            int sub_j = 0;
            for (int j = 0; j < n; j++) {
                if (j != col) {
                    submatrix->data[i-1][sub_j] = m->data[i][j];
                    sub_j++;
                }
            }
        }

        // Calculate cofactor
        double sign = (col % 2 == 0) ? 1.0 : -1.0;
        det += sign * m->data[0][col] * float_determinant(submatrix);

        dealloc_float_matrix(submatrix);
    }
    return det;
}


FloatMatrix* float_multiply_matrix(FloatMatrix *A, FloatMatrix *B) {
    if (A->cols != B->rows) {
        printf("Cannot multiply: A.cols (%d) != B.rows (%d)\n", A->cols, B->rows);
        return NULL;
    }

    FloatMatrix *C = create_float_matrix(A->rows, B->cols);
    if (C == NULL) {
        return NULL;
    }

    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->cols; j++) {
            double sum = 0.0;
            for (int k = 0; k < A->cols; k++) {
                sum += A->data[i][k] * B->data[k][j];
            }
            C->data[i][j] = sum;
        }
    }

    return C;
}

FloatMatrix* float_matrix_inverse(FloatMatrix *m) {
    if (m->rows != m->cols) {
        printf("Cannot invert non-square matrix\n");
        return NULL;
    }

    int n = m->rows;

    double det = float_determinant(m);
    if (fabs(det) < 1e-10) {
        printf("Matrix is singular (determinant ≈ 0), cannot be inverted\n");
        printf("Determinant: %.10e\n", det);
        return NULL;
    }

    FloatMatrix *augmented = create_float_matrix(n, 2 * n);
    if (augmented == NULL) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented->data[i][j] = m->data[i][j];
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented->data[i][n + j] = (i == j) ? 1.0 : 0.0;
        }
    }

    for (int col = 0; col < n; col++) {
        int pivot_row = col;
        double max_val = fabs(augmented->data[col][col]);

        for (int i = col + 1; i < n; i++) {
            double abs_val = fabs(augmented->data[i][col]);
            if (abs_val > max_val) {
                max_val = abs_val;
                pivot_row = i;
            }
        }

        if (fabs(augmented->data[pivot_row][col]) < 1e-10) {
            printf("Matrix is singular, cannot be inverted\n");
            dealloc_float_matrix(augmented);
            return NULL;
        }

        if (pivot_row != col) {
            for (int j = 0; j < 2 * n; j++) {
                double temp = augmented->data[col][j];
                augmented->data[col][j] = augmented->data[pivot_row][j];
                augmented->data[pivot_row][j] = temp;
            }
        }

        double pivot = augmented->data[col][col];
        for (int j = 0; j < 2 * n; j++) {
            augmented->data[col][j] = augmented->data[col][j] / pivot;
        }

        for (int i = 0; i < n; i++) {
            if (i != col) {
                double factor = augmented->data[i][col];
                for (int j = 0; j < 2 * n; j++) {
                    augmented->data[i][j] = augmented->data[i][j] - (factor * augmented->data[col][j]);
                }
            }
        }
    }

    FloatMatrix *inverse = create_float_matrix(n, n);
    if (inverse == NULL) {
        dealloc_float_matrix(augmented);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inverse->data[i][j] = augmented->data[i][n + j];
        }
    }

    dealloc_float_matrix(augmented);
    return inverse;
}


void test_float_determinant() {
    printf("\n=== Testing FloatMatrix Determinant ===\n");

    FloatMatrix *m = create_float_matrix(3, 3);
    m->data[0][0] = 6.0; m->data[0][1] = 1.0; m->data[0][2] = 1.0;
    m->data[1][0] = 4.0; m->data[1][1] = -2.0; m->data[1][2] = 5.0;
    m->data[2][0] = 2.0; m->data[2][1] = 8.0; m->data[2][2] = 7.0;
    printf("Matrix:\n");
    float_matrix_print(m);

    double det = float_determinant(m);
    printf("Determinant: %.2f (expected: -306.00)\n", det);

    dealloc_float_matrix(m);
}

void test_float_inverse() {
    printf("\n=== Testing FloatMatrix Inverse ===\n");

    FloatMatrix *m = create_float_matrix(2, 2);
    m->data[0][0] = 4.0; m->data[0][1] = 7.0;
    m->data[1][0] = 2.0; m->data[1][1] = 6.0;

    printf("Original Matrix:\n");
    float_matrix_print(m);

    double det = float_determinant(m);
    printf("Determinant: %.2f (matrix is invertible)\n\n", det);

    FloatMatrix *inv = float_matrix_inverse(m);
    if (inv != NULL) {
        printf("Inverse Matrix:\n");
        float_matrix_print(inv);

        printf("Expected:\n");
        printf("  0.6000  -0.7000\n");
        printf(" -0.2000   0.4000\n\n");

        FloatMatrix *identity = float_multiply_matrix(m, inv);
        if (identity != NULL) {
            printf("Verification (A * A^-1, should be identity):\n");
            float_matrix_print(identity);

            printf("Expected:\n");
            printf("  1.0000   0.0000\n");
            printf("  0.0000   1.0000\n");

            dealloc_float_matrix(identity);
        }

        dealloc_float_matrix(inv);
    }

    dealloc_float_matrix(m);
}
#ifndef NO_FLOAT_MAIN

int main() {
    printf("FloatMatrix Library Test\n");
    printf("========================\n");

    test_float_determinant();
    test_float_inverse();

    printf("\n✓ All FloatMatrix tests completed!\n");
    return 0;
}
#endif