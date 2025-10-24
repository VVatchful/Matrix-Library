#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  int rows;
  int cols;
  int **data;
} Matrix;

Matrix* create_matrix(int r,int c) {
  Matrix *m = (Matrix *)malloc(sizeof(Matrix));
  if (m == NULL) {
    perror("Failed to allocate memory to the Matrix");
    return NULL;
  }

  m->rows = r;
  m->cols = c;

  m->data = (int **)malloc(r * sizeof(int *));
  if (m->data == NULL) {
    perror("Failed to allocate memory for data rows");
    free(m);
    return NULL;
  }

  for (int i = 0; i < r; i++) {
    m->data[i] = (int *)malloc(c * sizeof(int));
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
  for(int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->data[i][j] = 0;
    }
  }
}

void init_random(Matrix *m, int min_value, int max_value) {
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      int rand_val = rand() % (max_value - min_value +1) + min_value;
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

Matrix* dupe_matrix(Matrix *m) {
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

Matrix* addition(Matrix *A, Matrix *B) {
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

int main() {

  srand(time(NULL));

  Matrix *_Matrix = create_matrix(2,3);
  if (_Matrix != NULL) {
    _Matrix->data[0][0] = 5;
    printf("Element at (0,0): %d\n", _Matrix->data[0][0]);
    dealloc_matrix(_Matrix);
  }
  return 0;

}


