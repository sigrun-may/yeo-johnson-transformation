/*H***************************************************************************
 * FILENAME : testFramework.c
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
 * AUTHOR   :       jbrenig           START DATE    : 12 September 2022
 *
 * CHANGES  :
 *
 * DATE     WHO     DETAIL
 *
 *H*/

/*****************************************************************************
 *                               INCLUDES
 *****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/lambdaSearch.h"
#include "include/timeStamps.h"
#include "include/vectorImports.h"
#include "include/yeoJohnson.h"

/*****************************************************************************
 *                           TEST FUNCTIONS
 *****************************************************************************/

int assert_int_equals(int a, int b, char *desc) {
  if (a == b) {
    return 0;
  } else {
    printf("%s\n", desc);
  }
  return -1;
}

int assert_double_equals(double a, double b, char *desc) {
  if (a == b) {
    return 0;
  } else {
    printf("%s\n", desc);
  }
  return -1;
}

int assert_float_equals(float a, float b, char *desc) {
  if (a == b) {
    return 0;
  } else {
    printf("%s\n", desc);
  }
  return -1;
}

int assert_string_equals(char *a, char *b, char *desc) {
  if (strcmp(a, b) == 0) {
    return 0;
  }
  printf("%s\n", desc);
  return -1;
}

int is_in_bound(double a, double origin, double range, char *desc) {
  if (a < origin + range && a > origin - range) {
    return 0;
  }
  printf("%s\n", desc);
  return -1;
}

int assert_not_null(void *ptr, char *desc) {
  if (ptr != NULL) {
    return 0;
  }
  printf("%s\n", desc);
  return -1;
}
