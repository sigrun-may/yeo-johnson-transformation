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
#include "include/vectorImports.h"

/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

int ciLambdaOperationOnMatrixFromFileS(char *file_path,
                                       double interval_start,
                                       double interval_end,
                                       double interval_step, MATRIX **return_matrix) {
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
  for (int i = 0; i < (*return_matrix)->cols; i++) {
    err_num = lsLambdaSearch(*((*return_matrix)->data + i), interval_start,
                             interval_end, interval_step, (*return_matrix)->rows,
                             &*((*return_matrix)->lambda + i), &*((*return_matrix)->skew + i));
    if (err_num != 0) {
      printf("abort on lambda search\n");
    }
  }
  return 0;
}

int ciLambdaOperation(double interval_start, double interval_end, double interval_step, MATRIX *input_matrix) {
  for (int i = 0; i < input_matrix->cols; i++) {
    int err_num = lsLambdaSearch(*(input_matrix->data + i), interval_start, interval_end, interval_step, input_matrix->rows, &*(input_matrix->lambda + i), &*(input_matrix->skew + i));
    if (err_num != 0) {
      printf("abort on lambda search\n");
    }
  }
  return 0;
}
