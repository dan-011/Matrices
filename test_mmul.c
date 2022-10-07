#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "square_matrix.h"
#include "unixtimer.h"


/************************************************************/
/* Do not change the code below                             */
/************************************************************/

#define DEFAULT_N           6
#define DEFAULT_NUM_THREADS 2

int main(int argc, char ** argv)
{
   size_t n = (argc < 2 ? DEFAULT_N : atol(argv[1]) );
   size_t num_threads = (argc < 3 ? DEFAULT_NUM_THREADS : atol(argv[2]) );

   square_matrix* m1 = new_square_matrix(n);
   assert(m1 != NULL);
   fill_square_matrix(m1);

   square_matrix* m2 = new_square_matrix(n);
   assert(m2 != NULL);
   fill_square_matrix(m2);

   start_timer();
   start_clock();
   square_matrix* res1 = mul_square_matrices(m1, m2);
   printf("Sequential time: %lf wall clock sec, %lf CPU sec\n", clock_seconds(), cpu_seconds() );
   assert(res1 != NULL);

   start_timer();
   start_clock();

   square_matrix* res2 = mul_square_matrices_threads(m1, m2, num_threads);
   printf("Threads time: %lf wall clock sec, %lf CPU sec\n", clock_seconds(), cpu_seconds() );
   assert(res2 != NULL);

   int r = compare_square_matrices(res1, res2);
   printf("%d %s\n", r, r ? "Do not match." : "Good work!");

   free_square_matrix(m1);
   free_square_matrix(m2);
   free_square_matrix(res1);
   free_square_matrix(res2);

   return 0;
}
