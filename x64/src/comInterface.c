/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT

 * FILENAME : comInterface.c
 *
 * DESCRIPTION  :
 * functional interface for construction of higher-order functions
 *
 * PUBLIC FUNCTIONS :
 * int ciLambdaOperationOnMatrixFromFileS(file_path, interval_start,
 *interval_end, interval_step, return_matrix, standardize, time_stamps) int
 *ciLambdaOperation(interval_start, interval_end, interval_step, input_matrix,
 *standardize, time_stamps) int ciSmartOperation(interval_start, interval_end,
 *precision, input_matrix, standardize, time_stamps) int
 *ciParallelOperation(interval_start, interval_end, precision, input_matrix,
 *standardize, time_stamps, thread_count)
 *
 * NOTES    :
 *
 *
 * AUTHOR   :       jbrenig           START DATE    : 14 September 2022
 *
 * CHANGES  :
 *
 * DATE     WHO     DETAIL
 *
 *H*/

/*****************************************************************************
 *                               INCLUDES
 *****************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#include "include/comInterface.h"
#include "include/lambdaSearch.h"
#include "include/timeStamps.h"
#include "include/vectorImports.h"
#include "include/yeoJohnson.h"

typedef struct _TBODY {
    double interval_start;
    double interval_end;
    int precision;
    MATRIX *input_matrix;
    int thread_count;
    int thread_number;
} TBODY;

/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

/**
 * @brief overwrites input vector with standardized vector of input
 *
 * @param vector input vector
 * @param rows row count of vector
 * @return int error return code
 */
static int ci_standardize(double *vector, int rows) {
    double avg = 0;
    double sd = 0;
    lsAverage(vector, rows, &avg);
    lsVariance(vector, avg, rows, &sd);
    for (int i = 0; i < rows; i++) {
        *(vector + i) = (*(vector + i) - avg) / sd;
    }
    return 0;
}

/**
 * @brief overwrites an array with vectors with standardized vectors of input
 *
 * @param input_matrix array of vectors
 * @return int error return code
 */
static int ci_do_standardize(MATRIX *input_matrix) {
    for (int i = 0; i < input_matrix->cols; i++) {
        ci_standardize(*(input_matrix->data + i), input_matrix->rows);
    }
    return 0;
}

/**
 * @brief thread entry function, thread executes function after creation for
 * ciParallelOperation matrix(array of vectors) is divided into modulo-classes,
 * each is handled by one thread
 *
 * @param args necessary information for calculation
 * @return void* pointer to thread information
 */
static void *threaded_operation(void *args) {
    TBODY *tb = (TBODY *) args;
    int err_num = 0;
    for (int i = tb->thread_number; i < tb->input_matrix->cols;
         i += tb->thread_count) {
        err_num = lsSmartSearch(
                *(tb->input_matrix->data + i), tb->interval_start, tb->interval_end,
                tb->precision, tb->input_matrix->rows, &*(tb->input_matrix->lambda + i),
                &*(tb->input_matrix->skew + i));
        if (err_num != 0) {
            printf("abort on lambda smart search\n");
        } else {
            err_num = yjTransformBy(&*(tb->input_matrix->data + i),
                                    *(tb->input_matrix->lambda + i),
                                    tb->input_matrix->rows);
            if (err_num != 0) {
                printf("abort on transformBy\n");
            }
        }
    }
    free(tb);
    pthread_exit(NULL);
    return NULL;
}

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief calculates lambda and skew for the matrix contained inside a file
 *
 * @param file_path path to file (.csv)
 * @param interval_start search interval start
 * @param interval_end search interval end
 * @param interval_step incrementation step
 * @param return_matrix matrix with lambda and skew vector
 * @param standardize bool if standardization is wished
 * @param time_stamps bool if time for calculation should be measured
 * @return int return error code
 */
int ciLambdaOperationOnMatrixFromFileS(char *file_path, double interval_start,
                                       double interval_end,
                                       double interval_step,
                                       MATRIX **return_matrix, BOOL standardize,
                                       BOOL time_stamps) {
    int err_num = 0;
    *(return_matrix) = (MATRIX *) malloc(sizeof(MATRIX));
    if (return_matrix == NULL) {
        printf("exception could not allocate memory for matrix\n");
        return -1;
    }
    err_num = importVectorTableFromCsv(file_path, &(*return_matrix));
    if (err_num != 0) {
        printf("abort on import of table from csv\n");
        return -1;
    }
    if (time_stamps) {
        // Starting Timer
        tsSetTimer();
    }
    for (int i = 0; i < (*return_matrix)->cols; i++) {
        err_num = lsLambdaSearch(
                *((*return_matrix)->data + i), interval_start, interval_end,
                interval_step, (*return_matrix)->rows, &*((*return_matrix)->lambda + i),
                &*((*return_matrix)->skew + i));
        if (err_num != 0) {
            printf("abort on lambda csv search\n");
        }
    }
    if (standardize) {
        // standardize vector list
        ci_do_standardize(*return_matrix);
    }
    if (time_stamps) {
        // Stopping Timer
        tsStopTimer();
        // printing result time
        double dt = 0;
        tsGetTime(&dt);
        printf("Time elapsed during transformation= %f s\n", dt);
    }
    return 0;
}

/**
 * @brief calculates lambda and skew for the input matrix
 *
 * @param interval_start search interval start
 * @param interval_end search interval end
 * @param interval_step incrementation step
 * @param input_matrix matrix to calculate lambas and skew
 * @param standardize bool if standardization is wished
 * @param time_stamps bool if time for calculation should be measured
 * @return int return error code
 */
int ciLambdaOperation(double interval_start, double interval_end,
                      double interval_step, MATRIX *input_matrix,
                      BOOL standardize, BOOL time_stamps) {
    int err_num = 0;
    if (time_stamps) {
        // Starting Timer
        tsSetTimer();
    }
    for (int i = 0; i < input_matrix->cols; i++) {
        err_num = lsLambdaSearch(*(input_matrix->data + i), interval_start,
                                 interval_end, interval_step, input_matrix->rows,
                                 &*(input_matrix->lambda + i),
                                 &*(input_matrix->skew + i));
        if (err_num != 0) {
            printf("abort on lambda search\n");
        } else {
            err_num = yjTransformBy(&*(input_matrix->data + i),
                                    *(input_matrix->lambda + i), input_matrix->rows);
            if (err_num != 0) {
                printf("abort on transformBy\n");
            }
        }
    }
    if (standardize) {
        // standardize vector list
        ci_do_standardize(input_matrix);
    }
    if (time_stamps) {
        // Stopping Timer
        tsStopTimer();
        // printing result time
        double dt = 0;
        tsGetTime(&dt);
        printf("Time elapsed during transformation= %f s\n", dt);
    }
    return 0;
}

/**
 * @brief calculates lambda and skew for the input matrix, different search
 * algorithm (scanning with defined precision) #can exit defined interval
 *
 * @param interval_start start of interval
 * @param interval_end end of interval
 * @param precision scanning precision
 * @param input_matrix array of vectors
 * @param standardize bool if standardization is wished
 * @param time_stamps bool if time for calculation should be measured
 * @return int error return code
 */
int ciSmartOperation(double interval_start, double interval_end, int precision,
                     MATRIX *input_matrix, BOOL standardize, BOOL time_stamps) {
    int err_num = 0;
    if (time_stamps) {
        // Starting Timer
        tsSetTimer();
    }
    for (int i = 0; i < input_matrix->cols; i++) {
        err_num =
                lsSmartSearch(*(input_matrix->data + i), interval_start, interval_end,
                              precision, input_matrix->rows,
                              &*(input_matrix->lambda + i), &*(input_matrix->skew + i));
        if (err_num != 0) {
            printf("abort on lambda smart search\n");
        } else {
            err_num = yjTransformBy(&*(input_matrix->data + i),
                                    *(input_matrix->lambda + i), input_matrix->rows);
            if (err_num != 0) {
                printf("abort on transformBy\n");
            }
        }
    }
    if (standardize) {
        // standardize vector list
        ci_do_standardize(input_matrix);
    }
    if (time_stamps) {
        // Stopping Timer
        tsStopTimer();
        // printing result time
        double dt = 0;
        tsGetTime(&dt);
        printf("Time elapsed during transformation= %f s\n", dt);
    }
    return 0;
}

/**
 * @brief calculates lambda and skew for the input matrix, different search
 * algorithm (scanning with defined precision) #can exit defined interval #multi
 * thread
 *
 * @param interval_start start of interval
 * @param interval_end end of interval
 * @param precision scanning precision
 * @param input_matrix array of vectors
 * @param standardize bool if standardization is wished
 * @param time_stamps bool if time for calculation should be measured
 * @param thread_count count of thread to be created
 * @return int error return code
 */
int ciParallelOperation(double interval_start, double interval_end,
                        int precision, MATRIX *input_matrix, BOOL standardize,
                        BOOL time_stamps, int thread_count) {
    // int err_num = 0;
    if (time_stamps) {
        // Starting Timer
        tsSetTimer();
    }
    // Thread instantiation
    if (thread_count <= 0) {
      printf("thread_count must be >= 1\n");
      return -1;
    } 
    pthread_t *th = malloc(sizeof(pthread_t) * thread_count);
    if (th == NULL) {
        printf("Not enough memory for thread creation\n");
        return -1;
    }
    
    for (int i = 0; i < thread_count; i++) {
        TBODY *tb = malloc(sizeof(TBODY));
        tb->input_matrix = input_matrix;
        tb->interval_start = interval_start;
        tb->interval_end = interval_end;
        tb->precision = precision;
        tb->thread_count = thread_count;
        tb->thread_number = i;
        pthread_create(&th[i], NULL, &threaded_operation, tb);
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(th[i], NULL);  // no memory leak -> memory is freed in threaded_operation
    }
    if (standardize) {
        // standardize vector list
        ci_do_standardize(input_matrix);
    }
    if (time_stamps) {
        // Stopping Timer
        tsStopTimer();
        // printing result time
        double dt = 0;
        tsGetTime(&dt);
        printf("Time elapsed during transformation= %f s\n", dt);
    }
    return 0;
}