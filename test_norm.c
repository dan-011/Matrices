#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
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

    assert(n > 0);

    square_matrix* m = new_square_matrix(n);
    assert(m != NULL);
    fill_square_matrix(m);

    start_timer();
    start_clock();
    long double norm0 = matrixNorm_by_col(m);
    assert( !isnan(norm0) );
    printf("Sequential time by column: %lf wall clock sec, %lf CPU sec\n", clock_seconds(), cpu_seconds() );
    printf("Sequential L2,1 norm by column: %Lf\n\n", norm0 );

    start_timer();
    start_clock();
    long double norm1 = matrixNorm(m);
    assert( !isnan(norm1) );
    printf("Sequential time by row: %lf wall clock sec, %lf CPU sec\n", clock_seconds(), cpu_seconds() );
    printf("Sequential L2,1 norm by row: %Lf\n\n", norm1 );

    for(int t=1; t<=num_threads; t = (2*t>num_threads && t<num_threads ? num_threads : 2*t) ) {
       start_timer();
       start_clock();
       long double norm2 = matrixNorm_threads(m, t);
       assert( !isnan(norm2) );
       printf("%d thread%s time: %lf wall clock sec, %lf CPU sec\n",
              t, (t==1 ? "" : "s"), clock_seconds(), cpu_seconds() );
       printf("%d thread%s L2,1 norm: %Lf\n",
              t, (t==1 ? "" : "s"), norm2 );
    }
    // clean up
    free_square_matrix(m);

    exit(0);
}
