#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "square_matrix.h"
#define BAND_SIZE 128

///////////////////////////////////////////////////////////////////////

// Define any necessary macros, types, and additional functions here
// TODO

/*
 * Transpose a square matrix
 *
 * Similar to transpose_square_matrix, but with multi-threading.
 */
typedef struct{
   size_t band_first_row;
   size_t band_last_row;
   size_t n;
   matrix_element** data;
   matrix_element** data2;
   
}thread_info;

void* threadf(void* arg){
   thread_info* thread = (thread_info*)arg;
   size_t band_first_row = thread->band_first_row;
   size_t band_last_row = thread->band_last_row;
   size_t n = thread->n;
   matrix_element** data = thread->data;
   matrix_element** data2 = thread->data2;

   // copy to the transpose  matrix rows band_first_row..band_last_row-1
   for(size_t j = 0; j < n; j++)
      for(size_t i = band_first_row; i < band_last_row; i++)
         data2[j][i] = data[i][j];
   pthread_exit(NULL);
}

square_matrix* transpose_square_matrix_threads(square_matrix* m, size_t num_threads)
{
   if(m == NULL)
      return NULL;

   // TODO
   size_t n = m->order;

   square_matrix* res = new_square_matrix(n);
   if(res == NULL)
      return NULL;

   matrix_element** data  = m->data;
   matrix_element** data2 = res->data;

   size_t band_first_row = 0;
   size_t band_last_row = 0;

   pthread_t tid[num_threads];
   thread_info arg[num_threads];

   size_t per_thread = n/num_threads;
   for(size_t i = 0; i < num_threads; i++){
      band_first_row = band_last_row;
      if(i == num_threads-1){
         band_last_row = n;
      }
      else{
         band_last_row += per_thread;
      }
      arg[i] = (thread_info){band_first_row, band_last_row, n, data, data2};
      //printf("THREADS start: %ld end: %ld\n", band_first_row, band_last_row);
      pthread_create(&tid[i], NULL, threadf, (void*)&arg[i]);
   }
   for(size_t i = 0; i < num_threads; i++){
      pthread_join(tid[i], NULL);
   }

   return res;
}

