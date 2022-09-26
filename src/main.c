#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/comInterface.h"
#include "include/timeStamps.h"


int main(int agrc, char *agrv[]) {
   // MATRIX *result = NULL;
   // ciLambdaOperationOnMatrixFromFileS("./data/Mappe3.CSV", -3, 3, 0.5, &result);
   // printf("cols=%i rows=%i\n", result->cols, result->rows);
   // for (int i = 0; i < result->cols; i++) {
   //      printf("col=%i lambda=%f skew=%f \n", i, *(result->lambda + i), *(result->skew + i));
   // }
   // printf("done");
   double v1[4] = {0,1,2,3};
   double v2[4] = {0,1,2,3};
   double v3[4] = {0,1,2,3};
   double *v_in[3] = {v1, v2, v3};
   double lambdas[3] = {0, 0, 0};
   double skews[3] = {-10, -10, -10};
   MATRIX result = {4, 3, v_in, lambdas, skews};
   int err_num = ciLambdaOperation(-3, 3, 0.5, &result);
   if (err_num != 0) {
      printf("exception during ciLambdaOperation\n");
   }
   for (int i = 0; i < 3; i++) {
      printf("i=%i | l=%.16f | s=%.16f\n", i, result.lambda[i], result.skew[i]);
   }
   return 0;
}