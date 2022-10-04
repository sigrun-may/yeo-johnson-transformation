/*H***************************************************************************
 * FILENAME : yeoJohnson.c
 *
 * DESCRIPTION  :
 *          Yeo Johnson transformation for specific values instead of vectors
 *          and all coherent sub functions.
 *
 * PUBLIC FUNCTIONS :
 *          int yjCalculation( double, double, double * )
 *          int yjCalculationf( float, float, float * )
 *
 * NOTES    :
 *          These functions are used to calculate a new distribution for
 *          a given vector of data.
 *          Given an input value and a lambda value the
 *          Yeo Johnson transformation is applied to said value.
 *
 * AUTHOR   :       jbrenig           START DATE    : 01 September 2022
 *
 * CHANGES  :
 *
 * DATE     WHO     DETAIL
 *
 *H*/

/*****************************************************************************
 *                               INCLUDES
 *****************************************************************************/

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/testFramework.h"
#include "include/yeoJohnson.h"

/*****************************************************************************
 *                               CONSTANTS
 *****************************************************************************/

static const double g_max_high_double = (double)0x7FFFFFFFFFFFFF;
static const double g_max_low_double = (double)0x80000000000000;
//static const double g_max_high_float = (float)0x7FFFFFFF;
//static const double g_max_low_float = (float)0x80000000;

/*****************************************************************************
 *                                GLOBALS
 *****************************************************************************/
// flags if boundary boxes have been set
static int bB_set = 0;
// boundary boxes
static boundaryBox bB_yj1;
static boundaryBox bB_yj3;

/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

/**
 * @brief (double) first formular of the Yeo Johnson transformation (y>=0,
 * lambda != 0)
 *
 * @param y value to be transformed
 * @param lambda transformation parameter
 * @param result resulting value after transformation
 * @return int error return code
 */
static int yjFormular1(double y, double lambda, double *result) {
  if (!bB_set) {
    printf("\tyjFormular1: boundary box is not set\n");
    return -1;
  }
  if (y > bB_yj1.upper_limit || y < bB_yj1.lower_limit) {
    printf("\tyjFormular1: value not inside boundary box\n");
    return -2;
  }
  *result = (pow(y + 1, lambda) - 1) / lambda;
  return 0;
}

/**
 * @brief (double) second formular of the Yeo Johnson transformation (y>=0,
 * lambda == 0)
 *
 * @param y value to be transformed
 * @param lambda transformation parameter
 * @param result resulting value after transformation
 * @return int error return code
 */
static int yjFormular2(double y, double *result) {
  if (y == g_max_high_double) {
    printf("\tyjFormular2: overflow y is equal to g_max_low_double\n");
    return -1;
  }
  *result = log(y + 1);
  return 0;
}

/**
 * @brief (double) third formular of the Yeo Johnson transformation (y<0, lambda
 * != 2)
 *
 * @param y value to be transformed
 * @param lambda transformation parameter
 * @param result resulting value after transformation
 * @return int error return code
 */
static int yjFormular3(double y, double lambda, double *result) {
  if (!bB_set) {
    printf("\tyjFormular3: boundary box is not set\n");
    return -1;
  }
  if (y > bB_yj3.upper_limit || y < bB_yj3.lower_limit) {
    printf("\tyjFormular3: value not inside boundary box/ y=%f, ul=%f, ll=%f\n",
           y, bB_yj3.upper_limit, bB_yj3.lower_limit);
    return -2;
  }
  *result = -(pow(-y + 1, 2 - lambda) - 1) / (2 - lambda);
  return 0;
}

/**
 * @brief (double) fourth formular of the Yeo Johnson transformation
 * (y<0, lambda == 2)
 *
 * @param y value to be transformed
 * @param lambda transformation parameter
 * @param result resulting value after transformation
 * @return int error return code
 */
static int yjFormular4(double y, double *result) {
  if (y == g_max_low_double) {
    printf("\tyjFormular4: overflow y is equal to g_max_low_double\n");
    return -1;
  }
  *result = -log(-y + 1);
  return 0;
}

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief (double) defines the boundaries of any value passed to yjCalculation
 *
 * @param lower_lambda lowest lambda possible for this search
 * @param upper_lambda highest lambda possible for this search
 */
void buildBoundaryBox(double lower_lambda, double upper_lambda) {
  upper_lambda = (upper_lambda == 0) ? 1.0 : upper_lambda;
  lower_lambda = (lower_lambda == 0) ? -1 : lower_lambda;
  bB_yj1.upper_limit =
      pow(upper_lambda * g_max_high_double + 1, 1 / upper_lambda) - 1;
  bB_yj1.lower_limit =
      pow(lower_lambda * g_max_low_double + 1, 1 / lower_lambda) - 1;

  upper_lambda = (upper_lambda == 2) ? 3 : upper_lambda;
  lower_lambda = (lower_lambda == 2) ? 1 : lower_lambda;
  bB_yj3.upper_limit = -(
      pow(-g_max_high_double * (2 - upper_lambda) + 1, 1 / (2 - upper_lambda)) -
      1);
  bB_yj3.lower_limit = -(
      pow(-g_max_low_double * (2 - lower_lambda) + 1, 1 / (2 - lower_lambda)) -
      1);
  bB_set = 1;
}

/**
 * @brief (double) Yeo Johnson transformation decision tree and error handling
 *
 * @param y value to be transformed
 * @param lambda transformation parameter
 * @param result resulting value after transformation
 * @return int error return code
 */
int yjCalculation(double y, double lambda, double *result) {
  int errnum;
  if (y >= 0) {
    if (lambda != 0) {
      errnum = yjFormular1(y, lambda, result);
      if (errnum != 0) {
        printf("\texception in yjFormular1\n");
        return -1;
      }
    } else if (lambda == 0) {
      errnum = yjFormular2(y, result);
      if (errnum != 0) {
        printf("\texception in yjFormular2\n");
        return -1;
      }
    }
  } else if (y < 0) {
    if (lambda != 2) {
      errnum = yjFormular3(y, lambda, result);
      if (errnum != 0) {
        printf("\texception in yjFormular3\n");
        return -1;
      }
    } else if (lambda == 2) {
      errnum = yjFormular4(y, result);
      if (errnum != 0) {
        printf("\texception in yjFormular4\n");
        return -1;
      }
    }
  }
  return 0;
}

int yjTransformBy(double **vector, double lambda, int rows) {
  for (int i = 0; i < rows; i++) {
    double result = 0;
    int err_num = yjCalculation(*((*vector) + i), lambda, &result);
    if (err_num != 0) {
      printf("\texception in yjTransformBy\n");
      return -1;
    }
    *((*vector) + i) = result;
  }
  return 0;
}

/*****************************************************************************
 *                                  TESTS
 *****************************************************************************/

#ifdef UNIT_TEST

void test_yj1(void) {
  bB_set = 0;
  double result;
  // test boundary box flag
  printf("Testing yjFormular1 in yeoJohnson.c\n");
  assert_int_equals(
      yjFormular1(0, 1, &result), -1,
      "Error: Boundary box for yj1 is not set but still executed");
  bB_set = 1;
  bB_yj1.lower_limit = -1;
  bB_yj1.upper_limit = 1;
  assert_int_equals(yjFormular1(0, 1, &result), 0,
                    "Error: Boundary box for yj1 is set but did not execute");
  // test limit
  assert_int_equals(
      yjFormular1(-2, 1, &result), -2,
      "Error: y=-2 is not inside limits [-1;1] but still executed");
  assert_int_equals(
      yjFormular1(2, 1, &result), -2,
      "Error: y=2 is not inside limits [-1;1] but still executed");
  assert_int_equals(yjFormular1(0, 1, &result), 0,
                    "Error: y=0 is inside limits [-1;1] but did not execute");
  // test formular
  bB_yj1.lower_limit = -500;
  bB_yj1.upper_limit = 500;
  yjFormular1(100, 2, &result);
  double expected = (pow(100 + 1, 2) - 1) / 2;
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj1f(void) {
  bBf_set = 0;
  float result;
  // test boundary box flag
  printf("Testing yjFormular1f in yeoJohnson.c\n");
  assert_int_equals(yjFormular1f(0, 1, &result), -1,
                    "Error: Boundary box is not set but still executed");
  bBf_set = 1;
  bBf_yj1.lower_limit = -1;
  bBf_yj1.upper_limit = 1;
  assert_int_equals(yjFormular1f(0, 1, &result), 0,
                    "Error: Boundary box is set but did not execute");
  // test limit
  assert_int_equals(
      yjFormular1f(-2, 1, &result), -2,
      "Error: y=-2 is not inside limits [-1;1] but still executed");
  assert_int_equals(
      yjFormular1f(2, 1, &result), -2,
      "Error: y=2 is not inside limits [-1;1] but still executed");
  assert_int_equals(yjFormular1f(0, 1, &result), 0,
                    "Error: y=0 is inside limits [-1;1] but did not execute");
  // test formular
  bBf_yj1.lower_limit = -500;
  bBf_yj1.upper_limit = 500;
  yjFormular1f(100, 2, &result);
  float expected = (powf(100 + 1, 2) - 1) / 2;
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj1U(void) {
  printf("Testing yjFormular1U in yeoJohnson.c\n");
  double result;
  yjFormular1U(100, 1, &result);
  double expected = (pow(100 + 1, 1) - 1) / 1;
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj1Uf(void) {
  printf("Testing yjFormular1Uf in yeoJohnson.c\n");
  float result;
  yjFormular1Uf(100, 1, &result);
  float expected = (powf(100 + 1, 1) - 1) / 1;
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj2(void) {
  double result;
  // test overflow
  printf("Testing yjFormular2 in yeoJohnson.c\n");
  assert_int_equals(yjFormular2(g_max_high_double, &result), -1,
                    "Error: y should overflow but still executed");
  // test success
  assert_int_equals(yjFormular2(2, &result), 0,
                    "Error: y=2 is inside limits but did not execute");
  // test formular
  yjFormular2(100, &result);
  double expected = log(100 + 1);
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj2f(void) {
  float result;
  // test overflow
  printf("Testing yjFormular2f in yeoJohnson.c\n");
  assert_int_equals(yjFormular2f(g_max_high_float, &result), -1,
                    "Error: y should overflow but still executed");
  // test success
  assert_int_equals(yjFormular2f(2, &result), 0,
                    "Error: y=2 is inside limits but did not execute");
  // test formular
  yjFormular2f(100, &result);
  float expected = logf(100 + 1);
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj2U(void) {
  double result;
  printf("Testing yjFormular2U in yeoJohnson.c\n");
  // test formular
  yjFormular2U(100, &result);
  double expected = log(100 + 1);
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj2Uf(void) {
  float result;
  printf("Testing yjFormular2Uf in yeoJohnson.c\n");
  // test formular
  yjFormular2Uf(100, &result);
  float expected = logf(100 + 1);
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj3(void) {
  bB_set = 0;
  double result;
  // test boundary box flag
  printf("Testing yjFormular3 in yeoJohnson.c\n");
  assert_int_equals(
      yjFormular3(-1, 1, &result), -1,
      "Error: Boundary box for yj3 is not set but still executed");
  bB_set = 1;
  bB_yj3.lower_limit = -2;
  bB_yj3.upper_limit = 2;
  assert_int_equals(yjFormular3(-1, 1, &result), 0,
                    "Error: Boundary box for yj3 is set but did not execute");
  // test limit
  assert_int_equals(
      yjFormular3(-3, 1, &result), -2,
      "Error: y=-3 is not inside limits [-2;2] but still executed");
  assert_int_equals(
      yjFormular3(3, 1, &result), -2,
      "Error: y=3 is not inside limits [-2;2] but still executed");
  assert_int_equals(yjFormular3(-1, 1, &result), 0,
                    "Error: y=-1 is inside limits [-2;2] but did not execute");
  // test formular
  bB_yj3.lower_limit = -500;
  bB_yj3.upper_limit = 500;
  yjFormular3(100, 3, &result);
  double expected = -(pow(-100 + 1, 2 - 3) - 1) / (2 - 3);
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj3f(void) {
  bBf_set = 0;
  float result;
  // test boundary box flag
  printf("Testing yjFormular3f in yeoJohnson.c\n");
  assert_int_equals(
      yjFormular3f(-1, 1, &result), -1,
      "Error: Boundary box for yj3f is not set but still executed");
  bBf_set = 1;
  bBf_yj3.lower_limit = -2;
  bBf_yj3.upper_limit = 2;
  assert_int_equals(yjFormular3f(-1, 1, &result), 0,
                    "Error: Boundary box for yj3f is set but did not execute");
  // test limit
  assert_int_equals(
      yjFormular3f(-3, 1, &result), -2,
      "Error: y=-3 is not inside limits [-2;2] but still executed");
  assert_int_equals(
      yjFormular3f(3, 1, &result), -2,
      "Error: y=3 is not inside limits [-2;2] but still executed");
  assert_int_equals(yjFormular3f(-1, 1, &result), 0,
                    "Error: y=-1 is inside limits [-2;2] but did not execute");
  // test formular
  bBf_yj3.lower_limit = -500;
  bBf_yj3.upper_limit = 500;
  yjFormular3f(-100, 3, &result);
  float expected = -(powf(-(-100) + 1, 2 - 3) - 1) / (2 - 3);
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj3U(void) {
  printf("Testing yjFormular3U in yeoJohnson.c\n");
  double result;
  yjFormular3U(-100, 3, &result);
  double expected = -(pow(-(-100) + 1, 2 - 3) - 1) / (2 - 3);
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj3Uf(void) {
  printf("Testing yjFormular3Uf in yeoJohnson.c\n");
  float result;
  yjFormular3Uf(-100, 3, &result);
  float expected = -(powf(-(-100) + 1, 2 - 3) - 1) / (2 - 3);
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj4(void) {
  double result;
  // test overflow
  printf("Testing yjFormular4 in yeoJohnson.c\n");
  assert_int_equals(yjFormular4(g_max_low_double, &result), -1,
                    "Error: y should overflow but still executed");
  // test success
  assert_int_equals(yjFormular4(2, &result), 0,
                    "Error: y=2 is inside limits but did not execute");
  // test formular
  yjFormular4(-100, &result);
  double expected = -log(-(-100) + 1);
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj4f(void) {
  float result;
  // test overflow
  printf("Testing yjFormular4f in yeoJohnson.c\n");
  assert_int_equals(yjFormular4f(g_max_low_float, &result), -1,
                    "Error: y should overflow but still executed");
  // test success
  assert_int_equals(yjFormular4f(2, &result), 0,
                    "Error: y=2 is inside limits but did not execute");
  // test formular
  yjFormular4f(-100, &result);
  float expected = -logf(-(-100) + 1);
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj4U(void) {
  double result;
  printf("Testing yjFormular4U in yeoJohnson.c\n");
  // test formular
  yjFormular4U(-100, &result);
  double expected = -log(-(-100) + 1);
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yj4Uf(void) {
  float result;
  printf("Testing yjFormular4U in yeoJohnson.c\n");
  // test formular
  yjFormular4Uf(-100, &result);
  float expected = -logf(-(-100) + 1);
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_buildBoundaryBox(void) {
  // double
  printf("Testing buildBoundaryBox in yeoJohnson.c\n");
  assert_int_equals(bB_set, 0,
                    "Error: boundary box flag is set 1 but should be 0");
  buildBoundaryBox(0, 0);
  assert_int_equals(bB_set, 1,
                    "Error: boundary box flag is set 0 but should be 1");
  assert_double_equals(
      bB_yj1.upper_limit, pow(1 * g_max_high_double + 1, 1 / 1) - 1,
      "Error: bB_yj1.upper_limit is 0 but should be corrected to 1");
  assert_double_equals(
      bB_yj1.lower_limit, pow(-1 * g_max_low_double + 1, 1 / -1) - 1,
      "Error: bB_yj1.lower_limit is 0 but should be corrected to -1");
  buildBoundaryBox(2, 2);
  assert_double_equals(
      bB_yj3.upper_limit,
      -(pow(-g_max_high_double * (2 - 3) + 1, 1 / (2 - 3)) - 1),
      "Error: bB_yj3.upper_limit is 2 but should be corrected to 3");
  assert_double_equals(
      bB_yj3.lower_limit,
      -(pow(-g_max_low_double * (2 - 1) + 1, 1 / (2 - 1)) - 1),
      "Error: bB_yj3.upper_limit is 2 but should be corrected to 1");
  printf("...done\n");

  // float
  printf("Testing buildBoundaryBoxf in yeoJohnson.c\n");
  assert_int_equals(bBf_set, 0,
                    "Error: boundary box flag is set 1 but should be 0");
  buildBoundaryBoxf(0, 0);
  assert_int_equals(bBf_set, 1,
                    "Error: boundary box flag is set 0 but should be 1");
  assert_double_equals(
      bBf_yj1.upper_limit, powf(1 * g_max_high_float + 1, 1 / 1) - 1,
      "Error: bBf_yj1.upper_limit is 0 but should be corrected to 1");
  assert_double_equals(
      bBf_yj1.lower_limit, powf(-1 * g_max_low_float + 1, 1 / -1) - 1,
      "Error: bBf_yj1.lower_limit is 0 but should be corrected to -1");
  buildBoundaryBoxf(2, 2);
  assert_double_equals(
      bBf_yj3.upper_limit,
      -(powf(-g_max_high_float * (2 - 3) + 1, 1 / (2 - 3)) - 1),
      "Error: bBf_yj3.upper_limit is 2 but should be corrected to 3");
  assert_double_equals(
      bBf_yj3.lower_limit,
      -(powf(-g_max_low_float * (2 - 1) + 1, 1 / (2 - 1)) - 1),
      "Error: bBf_yj3.upper_limit is 2 but should be corrected to 1");
  printf("...done\n");
}

void test_yjCalculation(void) {
  double result;
  printf("Testing yjCalculation in yeoJohnson.c\n");
  bB_set = 0;
  assert_int_equals(yjCalculation(0, 1, &result), -1,
                    "Error: exception in yjFormular1 did not get triggered");
  assert_int_equals(yjCalculation(g_max_high_double, 0, &result), -1,
                    "Error: exception in yjFormular2 did not get triggered");
  assert_int_equals(yjCalculation(-1, 1, &result), -1,
                    "Error: exception in yjFormular3 did not get triggered");
  assert_int_equals(yjCalculation(g_max_low_double, 2, &result), -1,
                    "Error: exception in yjFormular4 did not get triggered");
  buildBoundaryBox(-2, 2);
  assert_int_equals(yjCalculation(0, 1, &result), 0,
                    "Error: yj1 exception triggered but should not");
  assert_double_equals(result, (pow(0 + 1, 1) - 1) / 1,
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculation(0, 0, &result), 0,
                    "Error: yj2 exception triggered but should not");
  assert_double_equals(result, log(0 + 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculation(-1, 1, &result), 0,
                    "Error: yj3 exception triggered but should not");
  assert_double_equals(result, -(pow(-(-1) + 1, 2 - 1) - 1) / (2 - 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculation(-1, 2, &result), 0,
                    "Error: yj4 exception triggered but should not");
  assert_double_equals(result, -log(-(-1) + 1),
                       "Error: result is not equal to expected");

  printf("...done\n");
}

void test_yjCalculationf(void) {
  float result;
  printf("Testing yjCalculationf in yeoJohnson.c\n");
  bBf_set = 0;
  assert_int_equals(yjCalculationf(0, 1, &result), -1,
                    "Error: exception in yjFormular1f did not get triggered");
  assert_int_equals(yjCalculationf(g_max_high_float, 0, &result), -1,
                    "Error: exception in yjFormular2f did not get triggered");
  assert_int_equals(yjCalculationf(-1, 1, &result), -1,
                    "Error: exception in yjFormular3f did not get triggered");
  assert_int_equals(yjCalculationf(g_max_low_float, 2, &result), -1,
                    "Error: exception in yjFormular4f did not get triggered");
  buildBoundaryBoxf(-2, 2);
  assert_int_equals(yjCalculationf(0, 1, &result), 0,
                    "Error: yjf1 exception triggered but should not");
  assert_double_equals(result, (powf(0 + 1, 1) - 1) / 1,
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationf(0, 0, &result), 0,
                    "Error: yjf2 exception triggered but should not");
  assert_double_equals(result, logf(0 + 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationf(-1, 1, &result), 0,
                    "Error: yjf3 exception triggered but should not");
  assert_double_equals(result, -(powf(-(-1) + 1, 2 - 1) - 1) / (2 - 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationf(-1, 2, &result), 0,
                    "Error: yjf4 exception triggered but should not");
  assert_double_equals(result, -logf(-(-1) + 1),
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yjCalculationU(void) {
  double result;
  printf("Testing yjCalculationU in yeoJohnson.c\n");
  bB_set = 0;
  buildBoundaryBox(-2, 2);
  assert_int_equals(yjCalculationU(0, 1, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, (pow(0 + 1, 1) - 1) / 1,
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationU(0, 0, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, log(0 + 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationU(-1, 1, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, -(pow(-(-1) + 1, 2 - 1) - 1) / (2 - 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationU(-1, 2, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, -log(-(-1) + 1),
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_yjCalculationUf(void) {
  float result;
  printf("Testing yjCalculationf in yeoJohnson.c\n");
  bBf_set = 0;
  buildBoundaryBoxf(-2, 2);
  assert_int_equals(yjCalculationUf(0, 1, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, (powf(0 + 1, 1) - 1) / 1,
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationUf(0, 0, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, logf(0 + 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationUf(-1, 1, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, -(powf(-(-1) + 1, 2 - 1) - 1) / (2 - 1),
                       "Error: result is not equal to expected");
  assert_int_equals(yjCalculationUf(-1, 2, &result), 0,
                    "Error: exception triggered but should not");
  assert_double_equals(result, -logf(-(-1) + 1),
                       "Error: result is not equal to expected");
  printf("...done\n");
}

#endif