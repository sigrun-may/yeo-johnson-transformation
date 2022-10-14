/*H***************************************************************************
 * FILENAME : comInterface.c
 *
 * DESCRIPTION  :
 *
 *
 * PUBLIC FUNCTIONS :
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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

static int ci_do_standardize(MATRIX *input_matrix) {
  for (int i = 0; i < input_matrix->cols; i++) {
    ci_standardize(*(input_matrix->data + i), input_matrix->rows);
  }
  return 0;
}

static void *threaded_operation(void *args) {
  TBODY *tb = (TBODY *) args;
  for (int i = tb->thread_number; i < tb->input_matrix->cols; i+= tb->thread_count) {
    lsSmartSearch(*(tb->input_matrix->data + i), tb->interval_start, tb->interval_end, tb->precision, tb->input_matrix->rows, &*(tb->input_matrix->lambda + i), &*(tb->input_matrix->skew + i));
    yjTransformBy(&*(tb->input_matrix->data + i),
                              *(tb->input_matrix->lambda + i), tb->input_matrix->rows);
  }
  free(tb);
  pthread_exit(NULL);
  return NULL;
}

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

int ciLambdaOperationOnMatrixFromFileS(char *file_path, double interval_start,
                                       double interval_end,
                                       double interval_step,
                                       MATRIX **return_matrix, BOOL standardize,
                                       BOOL time_stamps) {
  *(return_matrix) = (MATRIX *)malloc(sizeof(MATRIX));
  if (return_matrix == NULL) {
    printf("exception could not allocate memory for matrix\n");
    return -1;
  }
  importVectorTableFromCsv(file_path, &(*return_matrix));
  if (time_stamps) {
    // Starting Timer
    tsSetTimer();
  }
  for (int i = 0; i < (*return_matrix)->cols; i++) {
    lsLambdaSearch(*((*return_matrix)->data + i), interval_start, interval_end,
                   interval_step, (*return_matrix)->rows,
                   &*((*return_matrix)->lambda + i),
                   &*((*return_matrix)->skew + i));
  }
  if (standardize) {
    // standardize vector list
    ci_do_standardize(*return_matrix);
  }
  if (time_stamps) {
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciLambdaOperation(double interval_start, double interval_end,
                      double interval_step, MATRIX *input_matrix,
                      BOOL standardize, BOOL time_stamps) {
  if (time_stamps) {
    // Starting Timer
    tsSetTimer();
  }
  for (int i = 0; i < input_matrix->cols; i++) {
    lsLambdaSearch(*(input_matrix->data + i), interval_start, interval_end,
                   interval_step, input_matrix->rows,
                   &*(input_matrix->lambda + i), &*(input_matrix->skew + i));
    yjTransformBy( &*(input_matrix->data + i), *(input_matrix->lambda + i),
                  input_matrix->rows);
  }
  if (standardize) {
    // standardize vector list
    ci_do_standardize(input_matrix);
  }
  if (time_stamps) {
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciSmartOperation(double interval_start, double interval_end, int precision,
                     MATRIX *input_matrix, BOOL standardize, BOOL time_stamps) {
  if (time_stamps) {
    // Starting Timer
    tsSetTimer();
  }
  for (int i = 0; i < input_matrix->cols; i++) {
    lsSmartSearch(*(input_matrix->data + i), interval_start, interval_end,
                  precision, input_matrix->rows, &*(input_matrix->lambda + i),
                  &*(input_matrix->skew + i));
    yjTransformBy(&*(input_matrix->data + i), *(input_matrix->lambda + i),
                  input_matrix->rows);
  }
  if (standardize) {
    // standardize vector list
    ci_do_standardize(input_matrix);
  }
  if (time_stamps) {
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciParallelOperation(double interval_start, double interval_end,
                        int precision, MATRIX *input_matrix, BOOL standardize,
                        BOOL time_stamps, int thread_count) {
  //int err_num = 0;
  if (time_stamps) {
    // Starting Timer
    tsSetTimer();
  }
  // Thread instantiation
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
    tb->precision =precision;
    tb->thread_count = thread_count;
    tb->thread_number = i;
    pthread_create(&th[i], NULL, &threaded_operation, tb);
  }
  for (int i = 0; i < thread_count; i++) {
    pthread_join(th[i], NULL);
  }
  if (standardize) {
    // standardize vector list
    ci_do_standardize(input_matrix);
  }
  if (time_stamps) {
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s\n", dt);
  }
  return 0;
}
