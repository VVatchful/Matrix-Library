#include <stdio.h>
#include <stdlib.h>

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

void deloc_matrix(Matrix *m) {
  if (m == NULL) return;
  for (int i = 0; i < m->rows; i++) {
    free(m->data[i]);
  }
  free(m->data);
  free(m);
}

int main() {
  Matrix *_Matrix = create_matrix(2,3);
  if (_Matrix != NULL) {
    _Matrix->data[0][0] = 5;
    printf("Element at (0,0): %d\n", _Matrix->data[0][0]);
    deloc_matrix(_Matrix);
  }
  return 0;

}

void fun(void)
{
}
