#define UNIT_TEST

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/comInterface.h"
#include "include/superTest.h"
#include "include/timeStamps.h"
#include "include/vectorImports.h"


int main(int agrc, char *agrv[]) {
  test_super_yj();
  test_super_vi();
  test_super_yj();
}