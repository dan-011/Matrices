#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "square_matrix2.h"

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

/*
 * Make a copy of a given square matrix.
 */
square_matrix* duplicate_square_matrix(square_matrix* m)
{
   if(m == NULL)
      return NULL;

   square_matrix* copy = new_square_matrix(m->order);
   if(copy == NULL)
      return NULL;

   // copy all elements with one memset since rows are contiguously allocated
   memcpy(copy->data[0], m->data[0], m->order * m->order * sizeof(matrix_element) );

   return copy;
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
         printf(" %2d", data[i][j]);
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


/////////////////////////////////////
//                                 //
// Sequential matrix addition      //
//                                 //
/////////////////////////////////////


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


/////////////////////////////////////
//                                 //
// Multi-threaded matrix addition  //
//                                 //
/////////////////////////////////////

typedef struct {
   size_t id, num_threads;
   square_matrix *m1, *m2, *res;
} thread_arg_t;


static void * thread_add(void * p_arg)
{
   thread_arg_t *p = p_arg;

   size_t id = p->id;
   size_t num_threads = p->num_threads;
   size_t n = p->m1->order;
   matrix_element** data1 = p->m1->data;
   matrix_element** data2 = p->m2->data;
   matrix_element** data  = p->res->data;

    // thread id will do rows:
    // id, id + num_threads, id + 2*num_threads, ...

    for(size_t i = id; i < n; i += num_threads)
       for(size_t j = 0; j < n; j ++)
          data[i][j] = data1[i][j] + data2[i][j];

    pthread_exit(NULL);
}


/*
 * Compute the sum of two square matrices. Return a pointer to the
 * newly allocated result matrix or NULL if anything is wrong
 *
 * Similar to add_square_matrices() in square_matrix.c but using multi-threading
 */
square_matrix* add_square_matrices_threads(square_matrix *m1, square_matrix *m2, size_t num_threads)
{

   if(m1 == NULL || m2 == NULL || m1->order != m2->order)
      return NULL;

   size_t n = m1->order;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   // adjust number of threads for small matrices
   num_threads = (n < num_threads) ? n : num_threads;
   pthread_t tid[num_threads];
   thread_arg_t args[num_threads];

   // prepare args and create threads
   for(size_t i = 0; i < num_threads; i ++) {
      args[i] = (thread_arg_t){i, num_threads, m1, m2, res};
      int status = pthread_create(&tid[i], NULL, thread_add, &args[i]);
      assert(status == 0); // could have handled errors better
   }

   // wait for threads to terminate
   for(size_t i = 0; i < num_threads; i ++)
      pthread_join(tid[i], NULL);

   return res;
}

//////////////////////////////////////
//                                  //
// Sequential matrix multiplication //
//                                  //
//////////////////////////////////////


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

//////////////////////////////////////////
//                                      //
// Multi-threaded matrix multiplication //
//                                      //
//////////////////////////////////////////

static void * thread_mul(void * p_arg)
{
   thread_arg_t *p = p_arg;

   size_t id = p->id;
   size_t num_threads = p->num_threads;
   size_t n = p->m1->order;
   matrix_element** data1 = p->m1->data;
   matrix_element** data2 = p->m2->data;
   matrix_element** data  = p->res->data;

   // thread id will do rows:
   // id, id + num_threads, id + 2*num_threads, ...

   for(size_t i = id; i < n; i += num_threads) {
      // zero out row i
      memset( &(data[i][0]), 0, n*sizeof(matrix_element) );

      // compute row i of product
      // Use IKJ order for best cache performance
      for(size_t k=0; k < n; k++)
         for(size_t j=0; j < n; j++)
            data[i][j] += data1[i][k] * data2[k][j];
    }

    pthread_exit(NULL);
}


/*
 * Compute the product of two square matrices. Return a pointer to the
 * newly allocated result matrix or NULL if anything is wrong
 *
 * Similar to add_square_matrices() in square_matrix.c but using multi-threading
 */
square_matrix* mul_square_matrices_threads(square_matrix *m1, square_matrix *m2, size_t num_threads)
{

   if(m1 == NULL || m2 == NULL || m1->order != m2->order)
      return NULL;

   size_t n = m1->order;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   // adjust number of threads for small matrices
   num_threads = (n < num_threads) ? n : num_threads;
   pthread_t tid[num_threads];
   thread_arg_t args[num_threads];

   // prepare args and create threads
   for(size_t i = 0; i < num_threads; i ++) {
      args[i] = (thread_arg_t){i, num_threads, m1, m2, res};
      int status = pthread_create(&tid[i], NULL, thread_mul, &args[i]);
      assert(status == 0); // could have handled errors better
   }

   // wait for threads to terminate
   for(size_t i = 0; i < num_threads; i ++)
      pthread_join(tid[i], NULL);

   return res;
}


//////////////////////////////////
//                              //
// Sequential matrix transpose  //
//                              //
//////////////////////////////////


/*
 * Compute the transpose of a square matrix. Return a pointer to the
 * newly allocated result matrix or NULL if anything is wrong
 */
square_matrix* transpose_square_matrix(square_matrix* m)
{
   if(m == NULL)
      return NULL;

   size_t n = m->order;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   matrix_element** data  = m->data;
   matrix_element** data2 = res->data;

   for(size_t i = 0; i < n; i++)
      for(size_t j = 0; j < n; j++)
         data2[j][i] = data[i][j];

   return res;
}

#define BAND_SIZE 128

/*
 * Compute the transpose of a square matrix. Return a pointer to the
 * newly allocated result matrix or NULL if anything is wrong.
 * Banded implementation with improved cache performance.
 */

square_matrix* transpose_square_matrix_banded(square_matrix* m)
{
   if(m == NULL)
      return NULL;

   size_t n = m->order;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   matrix_element** data  = m->data;
   matrix_element** data2 = res->data;


   // column-by-column copying done in bands to improve cache efficiency
   for(size_t band_first_row = 0; band_first_row < n; band_first_row += BAND_SIZE) {

      size_t band_last_row = band_first_row + BAND_SIZE;
      if(band_last_row > n) band_last_row = n;
      // copy to the transpose  matrix rows band_first_row..band_last_row-1
      //printf("SERIAL start: %ld end: %ld\n", band_first_row, band_last_row);
      for(size_t j = 0; j < n; j++)
         for(size_t i = band_first_row; i < band_last_row; i++)
             data2[j][i] = data[i][j];
   }

   return res;
}

