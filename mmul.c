#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "square_matrix.h"

///////////////////////////////////////////////////////////////////////

// Define any necessary macros, types, and additional functions here
// TODO
typedef struct{
   unsigned int row_start;
   unsigned int column_start;
   unsigned int row_end;
   unsigned int column_end;
   unsigned int n;
   matrix_element** m1_data;
   matrix_element** m2_data;
   matrix_element** mul_data;

}threadinfo;

void* mul_matrix(void* arg){
   threadinfo* argument = (threadinfo*)arg;
   unsigned int row_start = argument->row_start;
   unsigned int column_start = argument->column_start;
   unsigned int row_end = argument->row_end;
   unsigned int column_end = argument->column_end;
   unsigned int n = argument->n;
   matrix_element** m1_data = argument->m1_data;
   matrix_element** m2_data = argument->m2_data;
   matrix_element** mul_data = argument->mul_data;
   int i = row_start;
   int j = column_start;
   while(i < n){
      while(j < n){
         for(size_t k = 0; k < n; k++){
            mul_data[i][j] += m1_data[i][k] * m2_data[k][j];
         }
         if(row_end == i && column_end == j){
            pthread_exit(NULL);
         }
         j++;
      }
      j = 0;
      i++;
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
   // TODO
   pthread_t tid[num_threads];
   threadinfo arg[num_threads];
   unsigned int n = m1->order;
   matrix_element** m1_data = m1->data;
   matrix_element** m2_data = m2->data;
   square_matrix* mul = new_square_matrix(n);
   matrix_element** mul_data = mul->data;
   unsigned int num_elems = n * n;
   unsigned int start_row = 0;
   unsigned int end_row = 0;
   unsigned int start_column = 0;
   unsigned int end_column = 0;
   unsigned int accum = 0;
   unsigned int elems_per_thread = num_elems/num_threads;
   for(size_t thread = 0; thread < num_threads; thread++){
      start_row = accum/n;
      start_column = accum%n;
      accum += elems_per_thread;
      if(thread == (num_threads - 1)){
         //printf("ifstatement 1\n");
         accum = num_elems;
         end_row = n-1;
         end_column = n-1;
      }
      else{
         //printf("ifstatement 2\n");
         end_row = (accum-1)/n;
         end_column = (accum-1)%n;
      }
      //printf("SR: %d ER: %d - SC: %d EC: %d - accum: %d\n", start_row, end_row, start_column, end_column, accum);
      arg[thread] = (threadinfo){start_row, start_column, end_row, end_column, n, m1_data, m2_data, mul_data};
      pthread_create(&tid[thread], NULL, mul_matrix, (void*)&arg[thread]);
   }
   for(int i = 0; i < num_threads; i++){
      pthread_join(tid[i], NULL);
   }
   /*printf("m1\n");
   for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
         printf("%d ", m1->data[i][j]);
      }
      printf("\n");
   }
   printf("\nm2\n");
   for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
         printf("%d ", m2->data[i][j]);
      }
      printf("\n");
   }
   printf("\nsum\n");
   for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
         printf("%d ", sum->data[i][j]);
      }
      printf("\n");
   }*/
   return mul;

}
