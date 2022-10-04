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

#include "include/comInterface.h"
#include "include/lambdaSearch.h"
#include "include/timeStamps.h"
#include "include/vectorImports.h"
#include "include/yeoJohnson.h"

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

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

int ciLambdaOperationOnMatrixFromFileS(char *file_path, float interval_start,
                                       float interval_end, float interval_step,
                                       MATRIXF **return_matrix,
                                       BOOL standardize, BOOL time_stamps) {
  *(return_matrix) = (MATRIXF *)malloc(sizeof(MATRIXF));
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
    double dt = 0; // todo maybe float
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciLambdaOperation(float interval_start, float interval_end,
                      float interval_step, MATRIXF *input_matrix,
                      BOOL standardize, BOOL time_stamps) {
  if (time_stamps) {
    // Starting Timer
    tsSetTimer();
  }
  for (int i = 0; i < input_matrix->cols; i++) {
    lsLambdaSearch(*(input_matrix->data + i), interval_start, interval_end,
                   interval_step, input_matrix->rows,
                   &*(input_matrix->lambda + i), &*(input_matrix->skew + i));
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
    double dt = 0; // todo maybe float
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}

int ciSmartOperation(float interval_start, float interval_end, int precision,
                     MATRIXF *input_matrix, BOOL standardize,
                     BOOL time_stamps) {
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
    double dt = 0; // todo maybe float
    tsGetTime(&dt);
    printf("Time elapsed during transformation= %f s", dt);
  }
  return 0;
}
