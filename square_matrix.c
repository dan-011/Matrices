#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "square_matrix.h"

/*
 * Allocate space for a square matrix of order n.
 * If the allocation is not successful, return NULL.
 * If the allocation is successful, the data field of the matrix
 * points to an array of pointers, and each pointer
 * in this array points to an array that holds matrix elements
 * in the corresponding matrix row.
 */
square_matrix* new_square_matrix(size_t n)
{
   // allocate matrix struct
   square_matrix* new_m = malloc(sizeof(square_matrix));
   if(new_m == NULL)
      return NULL;

   matrix_element** data = malloc(n * sizeof(matrix_element*)); // array of row pointers
   if(data == NULL) {
      free(new_m);
      return NULL;
   }

   // allocate space for all matrix elements in one call
   matrix_element* storage = malloc(n * n * sizeof(matrix_element));
   if(storage == NULL) {
      free(new_m);
      free(data);
      return NULL;
   }

   // set row array pointers
   for(size_t i = 0; i < n; i++)
       data[i] = storage + i * n;

   new_m->order = n;
   new_m->data  = data;

   return new_m;
}

/*
 * Deallocate the dynamic memory allocated for the given matrix.
 */
void free_square_matrix(square_matrix* m)
{
   if(m == NULL)
        return;

   if(m->data) {
      free(m->data[0]);  // free the storage allocated for data
      free(m->data);     // free array of row pointers
   }

   free(m);             // free the matrix struct
}

#define MODULUS 7
/*
 *  Fill given matrix with random values
 */
void fill_square_matrix(square_matrix* m)
{
   // use static var to ensure srand is called only once
   static int first=1;

   if(first) {
      srand(3100);
      first = 0;
   }

   size_t n = m->order;
   matrix_element** data = m->data;

   for(size_t i = 0; i < n; i ++)
      for(size_t j = 0; j < n; j ++)
         data[i][j] = (matrix_element) rand() % MODULUS;
}


/*
 * Print given matrix row-by-row
 */
void print_square_matrix(square_matrix* m)
{
   if(m == NULL || m->data == NULL)
      return;

   size_t n = m->order;
   matrix_element** data = m->data;

   for(size_t i = 0; i < n; i++) {
      for(size_t j = 0; j < n; j++)
         printf("%10d", data[i][j]);
      printf("\n");
   }
}


/*
 * Compare two square matrices, return 0 if they are the same,
 * non-zero values otherwise.
 */
int compare_square_matrices(square_matrix* m1, square_matrix* m2)
{
   if(m1 == NULL || m2 == NULL)
      return -1;

   if(m1->order != m2->order)
      return -2;

   size_t n = m1->order;
   matrix_element** data1 = m1->data;
   matrix_element** data2 = m2->data;

   for(size_t i = 0; i < n; i ++)
      for(size_t j = 0; j < n; j ++)
         if(data1[i][j] != data2[i][j]) {
            fprintf(stderr, "Mismatch found for row %lu and column %lu: %d vs %d\n",
                    i, j, data1[i][j], data2[i][j]);
            return 1;
         }

   return 0;
}


/*
 * Compute the sum of two square matrices. Return a pointer to the
 * newly allocated result matrix or NULL if anything is wrong
 */
square_matrix* add_square_matrices(square_matrix* m1, square_matrix* m2)
{
   if(m1 == NULL || m2 == NULL || m1->order != m2->order)
      return NULL;

   size_t n = m1->order;
   matrix_element** data1 = m1->data;
   matrix_element** data2 = m2->data;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   matrix_element** data = res->data;
   for(size_t i = 0; i < n; i++)
      for(size_t j = 0; j < n; j++)
         data[i][j] = data1[i][j] + data2[i][j];

   return res;
}


/*
 * Compute the product of two square matrices. Return a pointer to the
 * newly allocated result matrix or NULL if anything is wrong
 */
square_matrix* mul_square_matrices(square_matrix* m1, square_matrix* m2)
{
   if(m1 == NULL || m2 == NULL || m1->order != m2->order)
      return NULL;

   size_t n = m1->order;
   matrix_element** data1 = m1->data;
   matrix_element** data2 = m2->data;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   matrix_element** data = res->data;

   // zero out result matrix with one memset since rows are contiguously allocated
   memset(&data[0][0], 0, n*n*sizeof(matrix_element));

   // Use IKJ order for best cache performance
   for(size_t i=0; i < n; i++)
      for(size_t k=0; k < n; k++)
         for(size_t j=0; j < n; j++)
            data[i][j] += data1[i][k] * data2[k][j];

   return res;
}

