#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "square_matrix.h"

///////////////////////////////////////////////////////////////////////

// Define any necessary macros, types, and additional functions here
// TODO



///////////////////////////////////////////////////////////////////////

/*
 * Compute and return the L2,1 norm of matrix m
 *
 * Return NAN if anything is wrong.
 *
 * Similar to matrixNorm, but using multi-threading.
 *
 */


typedef struct{
    size_t start_row;
    size_t end_row;
    size_t n;
    long double* sq_sum;
    matrix_element** data;
}thread_info;

void* threadf(void* arg){
    thread_info* thread = (thread_info*)arg;
    size_t start_row = thread->start_row;
    size_t end_row = thread->end_row;
    size_t n = thread->n;
    long double* sq_sum = thread->sq_sum;
    matrix_element** data = thread->data;
    //printf("Start row: %ld End row: %ld Total rows: %ld\n", start_row, end_row, n);
    for(size_t i = start_row; i < end_row; i++) {
        for(size_t j = 0; j < n; j++){
            sq_sum[j] += data[i][j] * data[i][j];
        }
    }
    pthread_exit(NULL);
}

long double matrixNorm_threads(square_matrix* m, size_t num_threads)
{
    if(m==NULL)
        return NAN;

    // TODO
    size_t n = m->order;
    matrix_element** data = m->data;

    // array holding the sum of squares for each column
    //long double *sq_sum = calloc(n, sizeof(long double)); // initializes with zeros

    // row-by-row processing for better spatial locality
    size_t start_row = 0;
    size_t end_row = 0;
    size_t per_thread = n/num_threads;
    thread_info arg[num_threads];
    pthread_t tid[num_threads];
    long double** sq_sums = calloc(num_threads, sizeof(long double));

    for(size_t i = 0; i < num_threads; i++){
        start_row = end_row;
        if(i == num_threads-1){
            end_row = n;
        }
        else{
            end_row += per_thread;
        }
        sq_sums[i] = (long double*)calloc(n, sizeof(long double));
        arg[i] = (thread_info){start_row, end_row, n, sq_sums[i], data};
        pthread_create(&tid[i], NULL, threadf, (void*)&arg[i]);
    }
    for(size_t i = 0; i < num_threads; i++){
        pthread_join(tid[i], NULL);
    }
    // compute L2,1 norm
    long double  norm = 0.0;
    long double val;
    for(size_t j = 0; j < n; j++){
        //printf("For %ld threads: adding %f from %Lf to %Lf\n", num_threads, sqrt(sq_sum[j]), sq_sum[j], norm);
        val = 0.0;
        for(size_t i = 0; i < num_threads; i++){
            val += sq_sums[i][j];
        }
        norm += sqrt(val);
    }

    // cleanup
    for(size_t i = 0; i < num_threads; i++){
        free(sq_sums[i]);
    }

    return norm;

}
