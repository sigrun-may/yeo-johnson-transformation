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

int ciLambdaOperationOnMatrixFromFileS(char *file_path, matrix *vector_field,
                                       double interval_start,
                                       double interval_end,
                                       double interval_step) {
  int err_num = 0;
  err_num = importVectorTableFromCsv(file_path, vector_field);
  if (err_num != 0) {
    printf("abort on import of table from csv\n");
    return -1;
  }
  for (int i = 0; i < vector_field->cols; i++) {
    err_num = lsLambdaSearch(*(vector_field->data + i), interval_start,
                             interval_end, interval_step, vector_field->rows,
                             vector_field->lambda + i, vector_field->skew + i);
    if (err_num != 0) {
      printf("abort on lambda search\n");
    }
  }
  return 0;
}
