#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/comInterface.h"
#include "include/timeStamps.h"


int main(int agrc, char *agrv[]) {
   float result = 3.402823466 * 100000000000000 * 100000000000000;
   printf("%f\n", result);
   return 0;
}