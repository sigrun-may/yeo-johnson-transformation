#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/comInterface.h"
#include "include/timeStamps.h"
#include "include/vectorImports.h"
#include "include/testFramework.h"


int main(int agrc, char *agrv[]) {
   MATRIX *mtx1 = malloc(sizeof(MATRIX));
   MATRIX *mtx2 = malloc(sizeof(MATRIX));
   importVectorTableFromCsv("C:\\Users\\jerom\\Documents\\vscode\\Yeo_Johnson\\x64\\data\\generated_data_close.csv", &mtx1);
   importVectorTableFromCsv("C:\\Users\\jerom\\Documents\\vscode\\Yeo_Johnson\\x64\\data\\generated_data_close.csv", &mtx2);
   printf("Start smart\n");
   ciSmartOperation(-3, 3, 14, mtx1, 1, 1);
   printf("Start threaded\n");
   ciParallelOperation(-3, 3, 14, mtx2, 1, 1 , 8);
   printf("Compare\n");
   int flag = 0;
   printf("Cols= %d , Rows= %d\n", mtx1->cols, mtx1->rows);
   for (int i = 0; i < mtx1->cols; i++) {
      if (is_in_bound(mtx1->lambda[i], mtx2->lambda[i], 0.0001, "Wrong\n")) {
         flag = 1;
      }
   }
   if (flag != 0) {
      printf("result not equal\n");
   } else {
      printf("success\n");
   }
   return 0;
}