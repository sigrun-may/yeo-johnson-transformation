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

typedef struct _TBODYF {
  float interval_start;
  float interval_end;
  int precision;
  MATRIXF *input_matrix;
  int thread_count;
  int thread_number;
} TBODYF;


/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

static int ci_standardize(float *vector, int rows) {
  float avg = 0;
  float sd = 0;
  lsAverage(vector, rows, &avg);
  lsVariance(vector, avg, rows, &sd);
  for (int i = 0; i < rows; i++) {
    *(vector + i) = (*(vector + i) - avg) / sd;
  }
  return 0;
}

static int ci_do_standardize(MATRIXF *input_matrix) {
  for (int i = 0; i < input_matrix->cols; i++) {
    ci_standardize(*(input_matrix->data + i), input_matrix->rows);
  }
  return 0;
}

static void *threaded_operation(void *args) {
  TBODYF *tb = (TBODYF *) args;
  int err_num = 0;
  for (int i = tb->thread_number; i < tb->input_matrix->cols; i+= tb->thread_count) {
    err_num = lsSmartSearch(*(tb->input_matrix->data + i), tb->interval_start, tb->interval_end, tb->precision, tb->input_matrix->rows, &*(tb->input_matrix->lambda + i), &*(tb->input_matrix->skew + i));
    if (err_num != 0) {
      printf("abort on lambda smart search\n");
    } else {
      err_num = yjTransformBy(&*(tb->input_matrix->data + i),
                              *(tb->input_matrix->lambda + i), tb->input_matrix->rows);
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

int ciLambdaOperationOnMatrixFromFileS(char *file_path, float interval_start,
                                       float interval_end,
                                       float interval_step,
                                       MATRIXF **return_matrix, BOOL standardize,
                                       BOOL time_stamps) {
  int err_num = 0;
  *(return_matrix) = (MATRIXF *)malloc(sizeof(MATRIXF));
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
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;  // todo maybe float
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciLambdaOperation(float interval_start, float interval_end,
                      float interval_step, MATRIXF *input_matrix,
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
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;  // todo maybe float
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciSmartOperation(float interval_start, float interval_end, int precision,
                     MATRIXF *input_matrix, BOOL standardize, BOOL time_stamps) {
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
    // Stoping Timer
    tsStopTimer();
    // printing result time
    double dt = 0;  // todo maybe float
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciParallelOperation(float interval_start, float interval_end,
                        int precision, MATRIXF *input_matrix, BOOL standardize,
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
    TBODYF *tb = malloc(sizeof(TBODYF));
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