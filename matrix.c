#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "matrix.h"

Matrix *create_matrix(int r, int c) {
  Matrix *m = (Matrix *) malloc(sizeof(Matrix));
  if (m == NULL) {
    perror("Failed to allocate memory to the Matrix");
    return NULL;
  }

  m->rows = r;
  m->cols = c;

  m->data = (int **) malloc(r * sizeof(int *));
  if (m->data == NULL) {
    perror("Failed to allocate memory for data rows");
    free(m);
    return NULL;
  }

  for (int i = 0; i < r; i++) {
    m->data[i] = (int *) malloc(c * sizeof(int));
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

void dealloc_matrix(Matrix *m) {
  if (m == NULL) return;
  for (int i = 0; i < m->rows; i++) {
    free(m->data[i]);
  }
  free(m->data);
  free(m);
}

void init_zero(Matrix *m) {
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->data[i][j] = 0;
    }
  }
}

void init_random(Matrix *m, int min_value, int max_value) {
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      int rand_val = rand() % (max_value - min_value + 1) + min_value;
      m->data[i][j] = rand_val;
    }
  }
}

void matrix_print(Matrix *m) {
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      printf("%d ", m->data[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

Matrix *dupe_matrix(Matrix *m) {
  Matrix *destination = create_matrix(m->rows, m->cols);

  if (destination == NULL) {
    return NULL;
  }

  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      destination->data[i][j] = m->data[i][j];
    }
  }
  return destination;
}

Matrix *addition(Matrix *A, Matrix *B) {
  if (A->rows != B->rows || A->cols != B->cols) {
    printf("Needs to be the same dimensions\n");
    return NULL;
  }

  Matrix *C = create_matrix(A->rows, A->cols);
  if (C == NULL) {
    return NULL;
  }

  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < A->cols; j++) {
      C->data[i][j] = A->data[i][j] + B->data[i][j];
    }
  }
  return C;
}

Matrix *scalar_multiply(Matrix *m, int scalar) {
  Matrix *result = create_matrix(m->rows, m->cols);

  if (result == NULL) {
    return NULL;
  }

  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      result->data[i][j] = m->data[i][j] * scalar;
    }
  }
  return result;
}

Matrix *transpose(Matrix *m) {
  Matrix *result = create_matrix(m->cols, m->rows);
  if (result == NULL) {
    return NULL;
  }

  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      result->data[j][i] = m->data[i][j];
    }
  }
  return result;
}

Matrix* multiply_matrix(Matrix *A, Matrix *B) {
  if (A->cols != B->rows) {
    printf("Cannot multiply: A.cols (%d) != B.rows (%d)\n", A->cols, B->rows);
    return NULL;
  }
  Matrix *C = create_matrix(A->rows, B->cols);
  if (C == NULL) {
    return NULL;
  }
  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < B->cols; j++) {
      int sum = 0;
      for (int k = 0; k < A->cols; k++) {
        sum = sum + (A->data[i][k] * B->data[k][j]);
      }
      C->data[i][j] = sum;
    }
  }
  return C;
}

double determinant(Matrix *m) {
  if (m->rows != m->cols) {
    printf("Determinant undefined for non-square matrices\n");
    return 0.0;
  }

  int n = m->rows;

  if (n == 1) {
    return (double)m->data[0][0];
  }
  if (n == 2) {
    return (double)(m->data[0][0] * m->data[1][1]) - (double)(m->data[0][1] * m->data[1][0]);
  }
  double det = 0.0;

  for (int col = 0; col < n; col++) {
    Matrix *submatrix = create_matrix(n - 1, n - 1);
    if (submatrix == NULL) {
      printf("Failed to create submatrix in determinant calculation\n");
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
    double sign = (col % 2 == 0) ? 1.0 : -1.0;
    double element = (double)m->data[0][col];
    det += sign * element * determinant(submatrix);

    dealloc_matrix(submatrix);
  }

  return det;
}

Matrix* matrix_inverse(Matrix *m) {
    if (m->rows != m->cols) {
        printf("Cannot invert non-square matrix\n");
        return NULL;
    }
    int n = m->rows;
    double det = determinant(m);
    if (det == 0.0) {
        printf("Matrix is singular (determinant = 0), cannot be inverted\n");
        return NULL;
    }
    Matrix *augmented = create_matrix(n, 2 * n);
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
            augmented->data[i][n + j] = (i == j) ? 1 : 0;
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
            dealloc_matrix(augmented);
            return NULL;
        }
        if (pivot_row != col) {
            for (int j = 0; j < 2 * n; j++) {
                int temp = augmented->data[col][j];
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
    Matrix *inverse = create_matrix(n, n);
    if (inverse == NULL) {
        dealloc_matrix(augmented);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inverse->data[i][j] = augmented->data[i][n + j];
        }
    }

    dealloc_matrix(augmented);
    return inverse;
}

void test_addition() {
  Matrix *A = create_matrix(2, 2);
  Matrix *B = create_matrix(2, 2);

  A->data[0][0] = 1, A->data[0][1] = 2;
  A->data[1][0] = 3, A->data[1][1] = 4;

  B->data[0][0] = 1, B->data[0][1] = 2;
  B->data[1][0] = 3, B->data[1][1] = 4;

  printf("Matrix A:\n");
  matrix_print(A);
  printf("Matrix B:\n");
  matrix_print(B);

  Matrix *C = addition(A, B);
  if (C != NULL) {
    printf("Matrix C (A + B):\n");
    matrix_print(C);
    dealloc_matrix(C);
  }

  dealloc_matrix(A);
  dealloc_matrix(B);
}

void test_determinant() {
    printf("\n=== Testing Determinant ===\n");

    // Test 2x2 matrix
    Matrix *m2 = create_matrix(2, 2);
    m2->data[0][0] = 4; m2->data[0][1] = 3;
    m2->data[1][0] = 2; m2->data[1][1] = 1;

    printf("Matrix 2x2:\n");
    matrix_print(m2);
    printf("Determinant: %.2f (expected: -2.00)\n", determinant(m2));

    dealloc_matrix(m2);

    // Test 3x3 matrix
    Matrix *m3 = create_matrix(3, 3);
    m3->data[0][0] = 1; m3->data[0][1] = 2; m3->data[0][2] = 3;
    m3->data[1][0] = 4; m3->data[1][1] = 5; m3->data[1][2] = 6;
    m3->data[2][0] = 7; m3->data[2][1] = 8; m3->data[2][2] = 9;

    printf("\nMatrix 3x3:\n");
    matrix_print(m3);
    printf("Determinant: %.2f (expected: 0.00 - singular matrix)\n", determinant(m3));

    dealloc_matrix(m3);
}

void test_inverse() {
    printf("\n=== Testing Matrix Inverse ===\n");

    Matrix *m = create_matrix(2, 2);
    m->data[0][0] = 4; m->data[0][1] = 7;
    m->data[1][0] = 2; m->data[1][1] = 6;

    printf("Original Matrix:\n");
    matrix_print(m);

    Matrix *inv = matrix_inverse(m);
    if (inv != NULL) {
        printf("Inverse Matrix:\n");
        matrix_print(inv);

        // Verify: A * A^-1 should equal I
        Matrix *identity = multiply_matrix(m, inv);
        if (identity != NULL) {
            printf("Verification (A * A^-1, should be identity):\n");
            matrix_print(identity);
            dealloc_matrix(identity);
        }

        dealloc_matrix(inv);
    }

    dealloc_matrix(m);
}

void test_multiply() {
    printf("\n=== Testing Matrix Multiplication ===\n");

    Matrix *A = create_matrix(2, 3);
    A->data[0][0] = 1; A->data[0][1] = 2; A->data[0][2] = 3;
    A->data[1][0] = 4; A->data[1][1] = 5; A->data[1][2] = 6;

    Matrix *B = create_matrix(3, 2);
    B->data[0][0] = 7; B->data[0][1] = 8;
    B->data[1][0] = 9; B->data[1][1] = 10;
    B->data[2][0] = 11; B->data[2][1] = 12;

    printf("Matrix A (2x3):\n");
    matrix_print(A);
    printf("Matrix B (3x2):\n");
    matrix_print(B);

    Matrix *C = multiply_matrix(A, B);
    if (C != NULL) {
        printf("Result C = A * B (2x2):\n");
        matrix_print(C);
        printf("Expected:\n");
        printf("58 64\n");
        printf("139 154\n");
        dealloc_matrix(C);
    }

    dealloc_matrix(A);
    dealloc_matrix(B);
}
#ifndef NO_MATRIX_MAIN
int main() {
  srand(time(NULL));

  printf("Matrix Library Test Suite\n");
  printf("===========================\n");

  test_addition();
  test_multiply();
  test_determinant();
  test_inverse();

  printf("\n✓ All tests completed!\n");
  return 0;
}
#endif
