#ifndef __square_matrix2_h__
#define __square_matrix2_h__


typedef int matrix_element;

typedef struct {
    size_t order;
    matrix_element** data;
} square_matrix;

square_matrix* new_square_matrix(size_t order);
void free_square_matrix(square_matrix* m);
square_matrix* duplicate_square_matrix(square_matrix* m);

void fill_square_matrix(square_matrix* m);
void print_square_matrix(square_matrix* m);
int  compare_square_matrices(square_matrix* m1, square_matrix* m2);

square_matrix* add_square_matrices(square_matrix* m1, square_matrix* m2);
square_matrix* mul_square_matrices(square_matrix* m1, square_matrix* m2);

square_matrix* add_square_matrices_threads(square_matrix* m1, square_matrix* m2, size_t num_threads);
square_matrix* mul_square_matrices_threads(square_matrix* m1, square_matrix* m2, size_t num_threads);

square_matrix* transpose_square_matrix(square_matrix* m);
square_matrix* transpose_square_matrix_banded(square_matrix* m);
square_matrix* transpose_square_matrix_threads(square_matrix* m, size_t num_threads);

#endif
