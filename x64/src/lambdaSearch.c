/****************************************************************
 * Copyright (c) 2023 Jerome Brenig, Sigrun May
 * Ostfalia Hochschule für angewandte Wissenschaften
 *
 * This software is distributed under the terms of the MIT license
 * which is available at https://opensource.org/licenses/MIT
 *
 * FILENAME : lambdaSearch.c
 *
 * DESCRIPTION  :
 *          Functions for calculation of the best fit lambda value,
 *          used for Yeo Johnson transformation.
 *
 * PUBLIC FUNCTIONS :
 *          int lsLambdaSearch ( double *, double, double, double, int, double*,
 *double * ) int lsLambdaSearchf ( double *, double, double, double, int, double
 **, double * )
 *
 * NOTES    :
 *          These functions are used inside the lambdaSearch function
 *          to find a specific lambda, that is used to adjust a vector
 *          of values to be more similar/closer to the normal distribution.
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

#include "include/errnumCodes.h"
#include "include/lambdaSearch.h"
#include "include/testFramework.h"
#include "include/yeoJohnson.h"


/*****************************************************************************
 *                               GLOBALS
 *****************************************************************************/

static const double g_maxHighDouble = __DBL_MAX__;

/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

/**
 * @brief (double) Calculating the average over the values of the given vector.
 *
 * @param vector containing all values
 * @param row_count the amount of contained values
 * @param result average
 * @return int error return code
 */
int lsAverage(double *vector, int row_count, double *result) {
  *result = 0;
  if (vector == NULL) {
    // printf("\tlsAverage: vector is null\n");
    return ERR_VECTOR_IS_NULL;
  }
  if (row_count <= 0) {
    // printf("\tlsAverage: not enough rows\n");
    return ERR_NOT_ENOUGH_ROWS;
  }
  double limit = g_maxHighDouble / row_count;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      // printf("\tlsAverage: component exeeded limit, abort for overflow "
      // "protection\n");
      return ERR_VALUE_OVERFLOW;
    }
    *result += component;
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (double) Calculating the variance over the values of the given vector.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param row_count the amount of contained values
 * @param result variance of the given vector
 * @return int error return code
 */
int lsVariance(double *vector, double average, int row_count, double *result) {
  *result = 0;
  if (vector == NULL) {
    // printf("\tlsVariance: vector is null\n");
    return ERR_VECTOR_IS_NULL;
  }
  if (row_count <= 1) {
    // printf("\tlsVariance: not enough rows\n");
    return ERR_NOT_ENOUGH_ROWS;
  }
  double limit = sqrt(g_maxHighDouble / row_count) + average;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      // printf("\tlsVariance: component exeeded limit, abort for overflow "
      // "protection\n");
      return ERR_VALUE_OVERFLOW;
    }
    *result += (component - average) * (component - average);
  }
  *result /= row_count - 1;
  *result = sqrt(*result);
  return 0;
}

/**
 * @brief (double) Calculating the skew over the values of the given vector.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param variance variance of the given vector
 * @param row_count the amount of contained values
 * @param result skew of the given vector
 * @return int error return code
 */
static int lsSkew(double *vector, double average, double variance,
                  int row_count, double *result) {
  *result = 0;
  if (vector == NULL) {
    // printf("\tlsSkew: vector is null\n");
    return ERR_VECTOR_IS_NULL;
  }
  if (row_count <= 2) {
    // printf("\tlsSkew:  not enough rows\n");
    return ERR_NOT_ENOUGH_ROWS;
  }
  double limit = cbrt(g_maxHighDouble / row_count) * variance + average;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      // printf("\tlsSkew:  component exeeded limit, abort for overflow "
      // "protection\n");
      return ERR_VALUE_OVERFLOW;
    }
    *result += ((component - average) / variance) *
               ((component - average) / variance) *
               ((component - average) / variance);
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (double) Measures the distance of two values to 0.
 *
 * @param value0 first value
 * @param value1 second value
 * @param result 1 if the first value is further away from 0 then the second
 * value, otherwise 0
 * @return int error return code
 */
static int lsIsCloserToZero(double value0, double value1, int *result) {
  value0 = value0 < 0 ? -value0 : value0;
  value1 = value1 < 0 ? -value1 : value1;
  if (value0 < value1) {
    *result = 0;
  } else if (value0 > value1) {
    *result = 1;
  } else {
    *result = 0;
  }
  return 0;
}

/**
 * @brief (double) Calculates the skew of the given vector and compares if a
 * given skew is closer to zero.
 *
 * @param vector    containing all values
 * @param row_count amount of contained values
 * @param skew  given skew
 * @param result 1 if the new skew is closer to 0, 0 otherwise
 * @return int error return code
 */
static int lsSkewIntervalStep(double *vector, int row_count, double *skew,
                              int *result) {
  *result = 0;
  double average;
  int errnum = lsAverage(vector, row_count, &average);
  if (errnum != 0) { // Mittelwert
    // printf("\texception in lsAverage: \n");
    return ERR_AVERAGE | errnum;
  }
  double sd;
  errnum = lsVariance(vector, average, row_count, &sd);
  if (errnum != 0) { // Standardabweichung
    // printf("\texception in lsVariance: \n");
    return ERR_DEVIATION | errnum;
  }
  double new_skew;
  errnum = lsSkew(vector, average, sd, row_count, &new_skew);
  if (errnum != 0) { // Schiefe
    // printf("\texception in lsSkew: \n");
    return ERR_SKEW | errnum;
  }
  int compare_flag;
  errnum = lsIsCloserToZero(*skew, new_skew, &compare_flag);
  if (errnum != 0) { // Vergleich mit vorheriger Schiefe
    // printf("\texception in lsIsCloserToZero: \n");
    return ERR_CLOSER_TO_ZERO | errnum;
  }
  if (compare_flag) {
    *skew = new_skew;
    *result = 1;
  }
  return 0;
}

/**
 * @brief (double) calculates the bowley skewness of three given parameters
 * 
 * @param q1 first quantil
 * @param q2 seconde quantil (median)
 * @param q3 third quantil
 * @param skew result variable for skewness
 * @param result 1 if the new skew is closer to 0, 0 otherwise
 * @return int error code
 */
static int lsBowleySkewnessStep(double q1, double q2, double q3, double  *skew, int *result) {
  *result = 0;
  if (q3-q1==0) {
    return ERR_SKEW_BOWLEY;
  }
  double new_skew = (q3+q1-2*q2)/(q3-q1);
  int compare_flag;
  int errnum = lsIsCloserToZero(*skew, new_skew, &compare_flag);
  if (errnum != 0) { // Vergleich mit vorheriger Schiefe
    return ERR_CLOSER_TO_ZERO | errnum;
  }
  if (compare_flag) {
    *skew = new_skew;
    *result = 1;
  }
  return 0;
}

/**
 * @brief sorts an array
 * 
 * @param vector array to be sorted
 * @param size size of the array
 * @return int error code
 */
// static int lsSortVector(double *vector, int size) {
//     double sort_arr[2][size];
//     int pointer_arr = 0;
//     memset(sort_arr[0], 0, size * sizeof(double));
//     memset(sort_arr[1], 0, size * sizeof(double));

//     for (int i = 0; i < size; i++) {
//         int j = 0;
//         int flag = 1;
//         while ((j < i) && flag) {
//             if (sort_arr[pointer_arr][j] <= vector[i]) {
//                 j++;
//             } else {
//                 for (int k = 0; k < j; k++) {
//                     sort_arr[getFlipped(pointer_arr)][k] = sort_arr[pointer_arr][k];
//                 }
//                 sort_arr[getFlipped(pointer_arr)][j] = vector[i];
//                 for (int k = j; k < i; k++) {
//                     sort_arr[getFlipped(pointer_arr)][k+1] = sort_arr[pointer_arr][k];
//                 }
//                 pointer_arr = getFlipped(pointer_arr);
//                 flag = 0;
//             }
//         }
//         if (j == i) sort_arr[pointer_arr][i] = vector[i];
//     }
//     for (int i = 0; i < size; i++) {
//         vector[i] = sort_arr[pointer_arr][i];
//     }
//     return 0;
// }

static int divide(double *vector, int left, int right) {
  int i = left;
  int j = right - 1;
  double pivot = vector[right];

  while ( i < j ) {
    while (i < j && vector[i] <= pivot) i++;
    while (j > i && vector[j] > pivot) j--;
    if (vector[i] > vector[j]) {
      double save = vector[i];
      vector[i] = vector[j];
      vector[j] = save;
    }
  }

  if (vector[i] > pivot) {
    double save = vector[i];
    vector[i] = vector[right];
    vector[right] = save;
  } else {
    i = right;
  }
  return i;
}

static void quickSort(double *vector, int left, int right) {
  if (left < right) {
    int divider = divide(vector, left, right);
    quickSort(vector, left, divider-1);
    quickSort(vector, divider+1, right);
  }
}

static int lsQuickSortVector(double *vector, int size) {
  quickSort(vector, 0, size-1);
  return 0;
}

/**
 * @brief gets the first, second and third quantil from a vector with a specific size
 * 
 * @param vector set of data (sorted)
 * @param size amount of entries
 * @param q1 first quantil
 * @param q2 second quantil (median)
 * @param q3 third quantil
 * @return int error code
 */
static int lsGetQuantils(double *vector, int size, double *q1, double *q2, double *q3) {
  double middle = size/2+0.5-1;
  double first_quarter = size/4-1;
  double last_quarter = size-first_quarter-1;
  // Getting median
  if (fmod(middle, 1)==0) {
    int pos = middle;
    *q2 = vector[pos];
  } else {
    int pos = middle-0.5;
    *q2 = (vector[pos] + vector[pos + 1])/2;
  }
  // Getting first quantil
  double leftover = fmod(first_quarter, 1);
  int pos = first_quarter-leftover;
  *q1 = vector[pos] * (1-leftover) + vector[pos + 1] * leftover;
  // Getting last quantil
  leftover = fmod(first_quarter, 1);
  pos = last_quarter-leftover;
  *q3 = vector[pos] * (1-leftover) + vector[pos + 1] * leftover;
  return 0;
}
/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief (double) Searching a lambda resulting in the skew closest to zero.
 *
 * @param vector containing all values
 * @param interval_start start value of the search
 * @param interval_end end value of the search
 * @param interval_step steps inside the interval
 * @param row_count amount of contained values
 * @param result_lambda lambda resulting in the skew closest to zero
 * @param result_skew skew which can be achieved with result_lambda
 * @return int error return code
 */
int lsLambdaSearch(double *vector, double interval_start, double interval_end,
                   double interval_step, int row_count, double *result_lambda,
                   double *result_skew, int *errnum) {
  *result_skew = g_maxHighDouble;
  double *zws = (double *)malloc(sizeof(double) * row_count);
  if (zws == NULL) {
    *errnum |= ERR_LAMBDA_SEARCH |
               ERR_FAILED_ALLOCATE_MEMORY; // memory allocation error
    // printf("\tFailed to allocate memory.\n");
    return -1;
  }
  memset(zws, 0, sizeof(double) * row_count); // is unsecure -> does not matter
                                              // is overwritten in yjCalculation
  *result_lambda = interval_start;
  buildBoundaryBox(interval_start, interval_end);
  int steps = ceil((interval_end - interval_start) / interval_step);
  for (int i = 0; i <= steps; i++) {
    double lambda_i = interval_start + (interval_step * i);
    for (int i = 0; i < row_count; i++) {
      *errnum |= yjCalculation(*(vector + i), lambda_i, zws + i);
      if (*errnum != 0) {
        *errnum |= ERR_LAMBDA_SEARCH | ERR_ABORT_YEO_JOHNSON;
        // printf("\texception occured during yeoJohnson\n");
        free(zws);
        return -2;
      }
    }
    int skew_test_flag;
    *errnum |=
        lsSkewIntervalStep(zws, row_count, &(*result_skew), &skew_test_flag);
    if (*errnum != 0) {
      *errnum |= ERR_LAMBDA_SEARCH | ERR_SKEW_TEST;
      // printf("\texception occured during skewTest\n");
      free(zws);
      return -3;
    }
    if (skew_test_flag) {
      *result_lambda = lambda_i;
    }
  }
  free(zws);
  return 0;
}

/**
 * @brief Searching a lambda resulting in the skew closest to zero by scanning
 * with precision instead of incremental steps
 *
 * @param vector input vector
 * @param interval_start start of interval
 * @param interval_end end of interval
 * @param precision scanning precision
 * @param row_count row count of vector
 * @param result_lambda lambda with skew closest to 0
 * @param result_skew resulting skew with calculated lambda
 * @return int error return code
 */
int lsSmartSearch(double *vector, double interval_start, double interval_end,
                  int precision, int row_count, double *result_lambda,
                  double *result_skew, int *errnum) {
  double *zws = (double *)malloc(sizeof(double) * row_count);
  if (zws == NULL) {
    *errnum |= ERR_LAMBDA_SEARCH | ERR_FAILED_ALLOCATE_MEMORY;
    // printf("\tFailed to allocate memory.\n");
    return -1;
  }
  buildBoundaryBox(interval_start, interval_end);
  double interval_step = 1;
  for (int s = 0; s <= precision; s++) {
    memset(zws, 0, sizeof(double) * row_count); // not secure -> does not matter
                                                // is overwritten yjCalculation
    *result_lambda = interval_start;
    *result_skew = g_maxHighDouble;
    int steps = ceil((interval_end - interval_start) / interval_step);
    for (int i = 0; i <= steps; i++) {
      double lambda_i = interval_start + (interval_step * i);
      for (int i = 0; i < row_count; i++) {
        *errnum |= yjCalculation(*(vector + i), lambda_i, zws + i);
        if (*errnum != 0) {
          *errnum |= ERR_LAMBDA_SEARCH | ERR_ABORT_YEO_JOHNSON;
          // printf("\texception occured during yeoJohnson\n");
          free(zws);
          return -2;
        }
      }
      int skew_test_flag;
      *errnum |=
          lsSkewIntervalStep(zws, row_count, &(*result_skew), &skew_test_flag);
      if (*errnum != 0) {
        *errnum |= ERR_LAMBDA_SEARCH | ERR_SKEW_TEST;
        // printf("\texception occured during skewTest\n");
        free(zws);
        return -3;
      }
      if (skew_test_flag) {
        *result_lambda = lambda_i;
      }
    }
    interval_start = *result_lambda - interval_step;
    interval_end = *result_lambda + interval_step;
    interval_step /= 2;
  }
  free(zws);
  *errnum = 0;
  return 0;
}

int lsSmartBowleySearch(double *vector, double interval_start, double interval_end,
                        int precision, int row_count, double *result_lambda,
double *result_skew, int *errnum) {
  double q1, q2, q3;
  double q1t, q2t, q3t;
  buildBoundaryBox(interval_start, interval_end);
  double interval_step = 1;
  lsQuickSortVector(vector, row_count);  // sort vector
  lsGetQuantils(vector, row_count, &q1, &q2, &q3); // get q1, q2, q3   
  for (int s = 0; s <= precision; s++) {
    *result_lambda = interval_start;
    *result_skew = g_maxHighDouble;
    int steps = ceil((interval_end - interval_start) / interval_step);
    for (int i = 0; i <= steps; i++) {
      double lambda_i = interval_start + (interval_step * i);
      *errnum |= yjCalculation(q1, lambda_i, &q1t);
      if (*errnum != 0) {
        *errnum |= ERR_LAMBDA_SEARCH | ERR_ABORT_YEO_JOHNSON;
        return -2;
      }
      *errnum |= yjCalculation(q2, lambda_i, &q2t);
      if (*errnum != 0) {
        *errnum |= ERR_LAMBDA_SEARCH | ERR_ABORT_YEO_JOHNSON;
        return -2;
      }
      *errnum |= yjCalculation(q3, lambda_i, &q3t);
      if (*errnum != 0) {
        *errnum |= ERR_LAMBDA_SEARCH | ERR_ABORT_YEO_JOHNSON;
        return -2;
      }
      int skew_test_flag;
      *errnum |=
          lsBowleySkewnessStep(q1t, q2t, q3t, result_skew, &skew_test_flag);
      if (*errnum != 0) {
        *errnum |= ERR_LAMBDA_SEARCH | ERR_SKEW_TEST;
        return -3;
      }
      if (skew_test_flag) {
        *result_lambda = lambda_i;
      }
    }
    interval_start = *result_lambda - interval_step;
    interval_end = *result_lambda + interval_step;
    interval_step /= 2;
  }
  return 0;        
}
/*****************************************************************************
 *                               TESTS
 *****************************************************************************/

#ifdef UNIT_TEST

void test_lsAverage(void) {
  printf("Testing lsAverage in lambdaSearch.c\n");
  double result;
  double vector[4] = {g_maxHighDouble, 1, 2, 3};
  assert_int_equals(lsAverage(NULL, 1, &result), -1,
                    "Error: vector is null, should abort");
  assert_int_equals(lsAverage(vector, 0, &result), -2,
                    "Error: row_count is <=0, should abort");
  assert_int_equals(lsAverage(vector, 4, &result), -3,
                    "Error: first value is too big, should abort");
  vector[0] = 0;
  assert_int_equals(lsAverage(vector, 4, &result), 0, "Error: should execute");
  assert_double_equals(result, 1.5, "Error: result should be 1.5");
  printf("...done\n");
}

void test_lsAveragef(void) {
  printf("Testing lsAveragef in lambdaSearch.c\n");
  float result;
  float vector[4] = {g_maxHighFloat, 1, 2, 3};
  assert_int_equals(lsAveragef(NULL, 1, &result), -1,
                    "Error: vector is null, should abort");
  assert_int_equals(lsAveragef(vector, 0, &result), -2,
                    "Error: row_count is <=0, should abort");
  assert_int_equals(lsAveragef(vector, 4, &result), -3,
                    "Error: first value is too big, should abort");
  vector[0] = 0;
  assert_int_equals(lsAveragef(vector, 4, &result), 0, "Error: should execute");
  assert_float_equals(result, 1.5, "Error: result should be 1.5");
  printf("...done\n");
}

void test_lsAverageU(void) {
  printf("Testing lsAverageU in lambdaSearch.c\n");
  double result;
  double vector[4] = {0, 1, 2, 3};
  assert_int_equals(lsAverage(vector, 4, &result), 0, "Error: should execute");
  assert_double_equals(result, 1.5, "Error: result should be 1.5");
  printf("...done\n");
}

void test_lsAverageUf(void) {
  printf("Testing lsAverageUf in lambdaSearch.c\n");
  float result;
  float vector[4] = {0, 1, 2, 3};
  assert_int_equals(lsAveragef(vector, 4, &result), 0, "Error: should execute");
  assert_float_equals(result, 1.5, "Error: result should be 1.5");
  printf("...done\n");
}

void test_lsVariance(void) {
  printf("Testing lsVariance in lambdaSearch.c\n");
  double result;
  double vector[4] = {g_maxHighDouble, 1, 2, 3};
  assert_int_equals(lsVariance(NULL, 1.5, 1, &result), -1,
                    "Error: vector is null, should abort");
  assert_int_equals(lsVariance(vector, 1.5, 1, &result), -2,
                    "Error: row_count is <=1, should abort");
  assert_int_equals(lsVariance(vector, 1.5, 4, &result), -3,
                    "Error: first value is too big, should abort");
  vector[0] = 0;
  assert_int_equals(lsVariance(vector, 1.5, 4, &result), 0,
                    "Error: should execute");
  assert_double_equals(result, sqrt((2.25 + 0.25 + 0.25 + 2.25) / 3),
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsVariancef(void) {
  printf("Testing lsVariancef in lambdaSearch.c\n");
  float result;
  float vector[4] = {g_maxHighFloat, 1, 2, 3};
  assert_int_equals(lsVariancef(NULL, 1.5, 1, &result), -1,
                    "Error: vector is null, should abort");
  assert_int_equals(lsVariancef(vector, 1.5, 1, &result), -2,
                    "Error: row_count is <=1, should abort");
  assert_int_equals(lsVariancef(vector, 1.5, 4, &result), -3,
                    "Error: first value is too big, should abort");
  vector[0] = 0;
  assert_int_equals(lsVariancef(vector, 1.5, 4, &result), 0,
                    "Error: should execute");
  assert_float_equals(result, sqrtf((2.25 + 0.25 + 0.25 + 2.25) / 3),
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsVarianceU(void) {
  printf("Testing lsVarianceU in lambdaSearch.c\n");
  double result;
  double vector[4] = {0, 1, 2, 3};
  assert_int_equals(lsVariance(vector, 1.5, 4, &result), 0,
                    "Error: should execute");
  assert_double_equals(result, sqrt((2.25 + 0.25 + 0.25 + 2.25) / 3),
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsVarianceUf(void) {
  printf("Testing lsVarianceUf in lambdaSearch.c\n");
  float result;
  float vector[4] = {0, 1, 2, 3};
  assert_int_equals(lsVariancef(vector, 1.5, 4, &result), 0,
                    "Error: should execute");
  assert_float_equals(result, sqrtf((2.25 + 0.25 + 0.25 + 2.25) / 3),
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsSkew(void) {
  printf("Testing lsSkew in lambdaSearch.c\n");
  double result;
  double average = 1.5;
  double variance = sqrt((2.25 + 0.25 + 0.25 + 2.25) / 3);
  double vector[4] = {g_maxHighDouble, 1, 2, 3};
  assert_int_equals(lsSkew(NULL, average, variance, 1, &result), -1,
                    "Error: vector is null, should abort");
  assert_int_equals(lsSkew(vector, average, variance, 1, &result), -2,
                    "Error: row_count is <=1, should abort");
  assert_int_equals(lsSkew(vector, average, variance, 4, &result), -3,
                    "Error: first value is too big, should abort");
  vector[0] = 0;
  assert_int_equals(lsSkew(vector, average, variance, 4, &result), 0,
                    "Error: should execute");
  double first_value = (0 - average) / variance * (0 - average) / variance *
                       (0 - average) / variance;
  double second_value = (1 - average) / variance * (1 - average) / variance *
                        (1 - average) / variance;
  double third_value = (2 - average) / variance * (2 - average) / variance *
                       (2 - average) / variance;
  double fourth_value = (3 - average) / variance * (3 - average) / variance *
                        (3 - average) / variance;
  double expected =
      (first_value + second_value + third_value + fourth_value) / 4;
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsSkewf(void) {
  printf("Testing lsSkewf in lambdaSearch.c\n");
  float result;
  float average = 1.5;
  float variance = sqrtf((2.25 + 0.25 + 0.25 + 2.25) / 3);
  float vector[4] = {g_maxHighFloat, 1, 2, 3};
  assert_int_equals(lsSkewf(NULL, average, variance, 1, &result), -1,
                    "Error: vector is null, should abort");
  assert_int_equals(lsSkewf(vector, average, variance, 1, &result), -2,
                    "Error: row_count is <=1, should abort");
  assert_int_equals(lsSkewf(vector, average, variance, 4, &result), -3,
                    "Error: first value is too big, should abort");
  vector[0] = 0;
  assert_int_equals(lsSkewf(vector, average, variance, 4, &result), 0,
                    "Error: should execute");
  float first_value = (0 - average) / variance * (0 - average) / variance *
                      (0 - average) / variance;
  float second_value = (1 - average) / variance * (1 - average) / variance *
                       (1 - average) / variance;
  float third_value = (2 - average) / variance * (2 - average) / variance *
                      (2 - average) / variance;
  float fourth_value = (3 - average) / variance * (3 - average) / variance *
                       (3 - average) / variance;
  float expected =
      (first_value + second_value + third_value + fourth_value) / 4;
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("result=%f ,expected= %f\n", result, expected);
  printf("...done\n");
}

void test_lsSkewU(void) {
  printf("Testing lsSkewU in lambdaSearch.c\n");
  double result;
  double average = 1.5;
  double variance = sqrt((2.25 + 0.25 + 0.25 + 2.25) / 3);
  double vector[4] = {0, 1, 2, 3};
  assert_int_equals(lsSkew(vector, average, variance, 4, &result), 0,
                    "Error: should execute");
  double first_value = (0 - average) / variance * (0 - average) / variance *
                       (0 - average) / variance;
  double second_value = (1 - average) / variance * (1 - average) / variance *
                        (1 - average) / variance;
  double third_value = (2 - average) / variance * (2 - average) / variance *
                       (2 - average) / variance;
  double fourth_value = (3 - average) / variance * (3 - average) / variance *
                        (3 - average) / variance;
  double expected =
      (first_value + second_value + third_value + fourth_value) / 4;
  assert_double_equals(result, expected,
                       "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsSkewUf(void) {
  printf("Testing lsSkewUf in lambdaSearch.c\n");
  float result;
  float average = 1.5;
  float variance = sqrtf((2.25 + 0.25 + 0.25 + 2.25) / 3);
  float vector[4] = {0, 1, 2, 3};
  assert_int_equals(lsSkewf(vector, average, variance, 4, &result), 0,
                    "Error: should execute");
  float first_value = (0 - average) / variance * (0 - average) / variance *
                      (0 - average) / variance;
  float second_value = (1 - average) / variance * (1 - average) / variance *
                       (1 - average) / variance;
  float third_value = (2 - average) / variance * (2 - average) / variance *
                      (2 - average) / variance;
  float fourth_value = (3 - average) / variance * (3 - average) / variance *
                       (3 - average) / variance;
  float expected =
      (first_value + second_value + third_value + fourth_value) / 4;
  assert_float_equals(result, expected,
                      "Error: result is not equal to expected");
  printf("...done\n");
}

void test_lsIsCloserToZero(void) {
  printf("Testing lsIsCloserToZero in lambdaSearch.c\n");
  int flag;
  assert_int_equals(lsIsCloserToZero(1, 1, &flag), 0,
                    "Error: should be executed");
  lsIsCloserToZero(1, 1, &flag);
  assert_int_equals(flag, 0, "Error: flag should be 0");
  lsIsCloserToZero(0, 1, &flag);
  assert_int_equals(flag, 0, "Error: flag should be 0");
  lsIsCloserToZero(2, 1, &flag);
  assert_int_equals(flag, 1, "Error: flag should be 1");
  printf("...done\n");
}

void test_lsIsCloserToZerof(void) {
  printf("Testing lsIsCloserToZerof in lambdaSearch.c\n");
  int flag;
  assert_int_equals(lsIsCloserToZerof(1, 1, &flag), 0,
                    "Error: should be executed");
  lsIsCloserToZerof(1, 1, &flag);
  assert_int_equals(flag, 0, "Error: flag should be 0");
  lsIsCloserToZerof(0, 1, &flag);
  assert_int_equals(flag, 0, "Error: flag should be 0");
  lsIsCloserToZerof(2, 1, &flag);
  assert_int_equals(flag, 1, "Error: flag should be 1");
  printf("...done\n");
}

void test_lsSkewIntervalStep(void) {
  printf("Testing lsSkewIntervalStep in lambdaSearch.c\n");
  double vector[4] = {0, 1, 2, 3};
  double skew = 10;
  int result = 0;
  assert_int_equals(lsSkewIntervalStep(vector, 0, &skew, &result), -1,
                    "Error: should abort during lsAverage");
  assert_int_equals(lsSkewIntervalStep(vector, 1, &skew, &result), -2,
                    "Error: should abort during lsVariance");
  assert_int_equals(lsSkewIntervalStep(vector, 2, &skew, &result), -3,
                    "Error: should abort during lsSkew");
  // assert_int_equals(lsSkewIntervalStep(vector, 0, &skew, &result), -4,
  // "Error: should abort during lsIsCloserToZero"); // no error possible
  assert_int_equals(lsSkewIntervalStep(vector, 4, &skew, &result), 0,
                    "Error: should execute");
  assert_double_equals(result, 1,
                       "Error: skew should be swapped and result should be 1");
  printf("...done\n");
}

void test_lsSkewIntervalStepf(void) {
  printf("Testing lsSkewIntervalStepf in lambdaSearch.c\n");
  float vector[4] = {0, 1, 2, 3};
  float skew = 10;
  int result = 0;
  assert_int_equals(lsSkewIntervalStepf(vector, 0, &skew, &result), -1,
                    "Error: should abort during lsAveragef");
  assert_int_equals(lsSkewIntervalStepf(vector, 1, &skew, &result), -2,
                    "Error: should abort during lsVariancef");
  assert_int_equals(lsSkewIntervalStepf(vector, 2, &skew, &result), -3,
                    "Error: should abort during lsSkewf");
  // assert_int_equals(lsSkewIntervalStepf(vector, 0, &skew, &result), -4,
  // "Error: should abort during lsIsCloserToZerof"); // no error possible
  assert_int_equals(lsSkewIntervalStepf(vector, 4, &skew, &result), 0,
                    "Error: should execute");
  assert_float_equals(result, 1,
                      "Error: skew should be swapped and result should be 1");
  printf("...done\n");
}

void test_lsSkewIntervalStepU(void) {
  printf("Testing lsSkewIntervalStepU in lambdaSearch.c\n");
  double vector[4] = {0, 1, 2, 3};
  double skew = 10;
  int result = 0;
  assert_int_equals(lsSkewIntervalStep(vector, 4, &skew, &result), 0,
                    "Error: should execute");
  assert_double_equals(result, 1,
                       "Error: skew should be swapped and result should be 1");
  printf("...done\n");
}

void test_lsSkewIntervalStepUf(void) {
  printf("Testing lsSkewIntervalStepUf in lambdaSearch.c\n");
  float vector[4] = {0, 1, 2, 3};
  float skew = 10;
  int result = 0;
  assert_int_equals(lsSkewIntervalStepf(vector, 4, &skew, &result), 0,
                    "Error: should execute");
  assert_float_equals(result, 1,
                      "Error: skew should be swapped and result should be 1");
  printf("...done\n");
}

void test_lsLambdaSearch(void) {
  printf("Testing lsLambdaSearch in lambdaSearch.c\n");
  double vector[4] = {g_maxHighDouble, 1, 2, 3};
  double result_lambda = 0;
  double result_skew = 10;
  assert_int_equals(
      lsLambdaSearch(vector, -1, 1, 1, 4, &result_lambda, &result_skew), -2,
      "Error: yjCalculation completed, should abort");
  assert_int_equals(
      lsLambdaSearch(vector, -1, 1, 1, 0, &result_lambda, &result_skew), -3,
      "Error: skewTest completed, should abort");
  vector[0] = 0;
  assert_int_equals(
      lsLambdaSearch(vector, -1, 1, 1, 4, &result_lambda, &result_skew), 0,
      "Error: should execute");
  printf("...done\n");
}

void test_lsLambdaSearchf(void) {
  printf("Testing lsLambdaSearchf in lambdaSearch.c\n");
  float vector[4] = {g_maxHighFloat, 1, 2, 3};
  float result_lambda = 0;
  float result_skew = 10;
  assert_int_equals(
      lsLambdaSearchf(vector, -1, 1, 1, 4, &result_lambda, &result_skew), -2,
      "Error: yjCalculation completed, should abort");
  assert_int_equals(
      lsLambdaSearchf(vector, -1, 1, 1, 0, &result_lambda, &result_skew), -3,
      "Error: skewTest completed, should abort");
  vector[0] = 0;
  assert_int_equals(
      lsLambdaSearchf(vector, -1, 1, 1, 4, &result_lambda, &result_skew), 0,
      "Error: should execute");
  printf("...done\n");
}

void test_lsLambdaSearchU(void) {
  printf("Testing lsLambdaSearchU in lambdaSearch.c\n");
  double vector[4] = {0, 1, 2, 3};
  double result_lambda = 0;
  double result_skew = 10;
  assert_int_equals(
      lsLambdaSearch(vector, -1, 1, 1, 4, &result_lambda, &result_skew), 0,
      "Error: should execute");
  printf("...done\n");
}

void test_lsLambdaSearchUf(void) {
  printf("Testing lsLambdaSearchUf in lambdaSearch.c\n");
  float vector[4] = {0, 1, 2, 3};
  float result_lambda = 0;
  float result_skew = 10;
  assert_int_equals(
      lsLambdaSearchf(vector, -1, 1, 1, 4, &result_lambda, &result_skew), 0,
      "Error: should execute");
  printf("...done\n");
}

#endif