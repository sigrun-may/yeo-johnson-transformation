/*H***************************************************************************
 * FILENAME : lambdaSearch.c
 *
 * DESCRIPTION  :
 *          Functions for calculation of the best fit lambda value,
 *          used for Yeo Johnson transformation.
 *
 * PUBLIC FUNCTIONS :
 *          int lsLambdaSearch ( double *, double, double, double, int, double
 **, double * ) int lsLambdaSearchf ( double *, double, double, double, int,
 *double *, double * )
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

#include "include/lambdaSearch.h"
#include "include/testFramework.h"
#include "include/yeoJohnson.h"

/*****************************************************************************
 *                               GLOBALS
 *****************************************************************************/

static const double g_maxHighDouble = (double)0x7FFFFFFFFFFFFF;
static const double g_maxLowDouble = (double)0x80000000000000;
static const double g_maxHighFloat = (float)0x7FFFFFFF;
static const double g_maxLowFloat = (float)0x80000000;

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
static int lsAverage(double *vector, int row_count, double *result) {
  if (vector == NULL) {
    printf("lsAverage: vector is null\n");
    return -1;
  }
  if (row_count <= 0) {
    printf("lsAverage: not enough rows\n");
    return -2;
  }
  double limit = g_maxHighDouble / row_count;
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      printf("lsAverage: component exeeded limit, abort for overflow "
             "protection\n");
      return -3;
    }
    *result += component;
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (float) Calculating the average over the values of the given vector.
 *
 * @param vector containing all values
 * @param row_count the amount of contained values
 * @param result average
 * @return int error return code
 */
static int lsAveragef(float *vector, int row_count, float *result) {
  if (vector == NULL) {
    printf("lsAveragef: vector is null\n");
    return -1;
  }
  if (row_count <= 0) {
    printf("lsAveragef: not enough rows\n");
    return -2;
  }
  float limit = g_maxHighFloat / row_count;
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      printf("lsAveragef: component exeeded limit, abort for overflow "
             "protection\n");
      return -3;
    }
    *result += component;
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (double) Calculating the average over the values of the given vector
 *         without intercepting any errors.
 *
 * @param vector containing all values
 * @param row_count the amount of contained values
 * @param result average
 * @return int error return code, always 0
 */
static int lsAverageU(double *vector, int row_count, double *result) {
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    *result += *(vector + i);
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (float) Calculating the average over the values of the given vector
 *         without intercepting any errors.
 *
 * @param vector containing all values
 * @param row_count the amount of contained values
 * @param result average of the given vector
 * @return int error return code, always 0
 */
static int lsAverageUf(float *vector, int row_count, float *result) {
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    *result += *(vector + i);
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
static int lsVariance(double *vector, double average, int row_count,
                      double *result) {
  if (vector == NULL) {
    printf("lsVariance: vector is null\n");
    return -1;
  }
  if (row_count <= 1) {
    printf("lsVariance: not enough rows\n");
    return -2;
  }
  *result = 0;
  double limit = sqrt(g_maxHighDouble / row_count) + average;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      printf("lsVariance: component=%f exeeded limit=%f, average=%f, abort for "
             "overflow protection\n",
             component, limit, average);
      return -3;
    }
    *result += (component - average) * (component - average);
  }
  *result /= row_count - 1;
  *result = sqrt(*result);
  return 0;
}

/**
 * @brief (float) Calculating the variance over the values of the given vector.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param row_count the amount of contained values
 * @param result variance of the given vector
 * @return int error return code
 */
static int lsVariancef(float *vector, float average, int row_count,
                       float *result) {
  if (vector == NULL) {
    printf("lsVariancef: vector is null\n");
    return -1;
  }
  if (row_count <= 1) {
    printf("lsVariancef: not enough rows\n");
    return -2;
  }
  *result = 0;
  float limit = sqrtf(g_maxHighFloat / row_count) + average;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      printf("lsVariancef: component exeeded limit, abort for overflow "
             "protection\n");
      return -3;
    }
    *result += (component - average) * (component - average);
  }
  *result /= row_count - 1;
  *result = sqrtf(*result);
  return 0;
}

/**
 * @brief (double) Calculating the variance over the values of the given vector
 *          without intercepting any errors.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param row_count the amount of contained values
 * @param result variance of the given vector
 * @return int error return code, always 0
 */
static int lsVarianceU(double *vector, double average, int row_count,
                       double *result) {
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    *result += (*(vector + i) - average) * (*(vector + i) - average);
  }
  *result /= row_count - 1;
  *result = sqrt(*result);
  return 0;
}

/**
 * @brief (float) Calculating the variance over the values of the given vector
 *          without intercepting any errors.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param row_count the amount of contained values
 * @param result variance of the given vector
 * @return int error return code, always 0
 */
static int lsVarianceUf(float *vector, float average, int row_count,
                        float *result) {
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    *result += (*(vector + i) - average) * (*(vector + i) - average);
  }
  *result /= row_count - 1;
  *result = sqrtf(*result);
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
  if (vector == NULL) {
    printf("lsSkew: vector is null\n");
    return -1;
  }
  if (row_count <= 2) {
    printf("lsSkew:  not enough rows\n");
    return -2;
  }
  double limit = cbrt(g_maxHighDouble / row_count) * variance + average;
  for (int i = 0; i < row_count; i++) {
    double component = *(vector + i);
    if (component > limit || component < -limit) {
      printf(
          "lsSkew:  component exeeded limit, abort for overflow protection\n");
      return -3;
    }
    *result += ((component - average) / variance) *
               ((component - average) / variance) *
               ((component - average) / variance);
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (float) Calculating the skew over the values of the given vector.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param variance variance of the given vector
 * @param row_count the amount of contained values
 * @param result skew of the given vector
 * @return int error return code
 */
static int lsSkewf(float *vector, float average, float variance, int row_count,
                   float *result) {
  if (vector == NULL) {
    printf("lsSkewf: vector is null\n");
    return -1;
  }
  if (row_count <= 2) {
    printf("lsSkew:  not enough rows\n");
    return -2;
  }
  float limit = cbrt(g_maxHighFloat / row_count) * variance + average;
  for (int i = 0; i < row_count; i++) {
    float component = *(vector + i);
    if (component > limit || component < -limit) {
      printf(
          "lsSkew:  component exeeded limit, abort for overflow protection\n");
      return -3;
    }
    *result += ((component - average) / variance) *
               ((component - average) / variance) *
               ((component - average) / variance);
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (double) Calculating the skew over the values of the given vector
 *          without intercepting any errors.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param variance variance of the given vector
 * @param row_count the amount of contained values
 * @param result skew of the given vector
 * @return int error return code, always 0
 */
static int lsSkewU(double *vector, double average, double variance,
                   int row_count, double *result) {
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    *result += ((*(vector + i) - average) / variance) *
               ((*(vector + i) - average) / variance) *
               ((*(vector + i) - average) / variance);
  }
  *result /= row_count;
  return 0;
}

/**
 * @brief (float) Calculating the skew over the values of the given vector
 *          without intercepting any errors.
 *
 * @param vector containing all values
 * @param average average of the given vector
 * @param variance variance of the given vector
 * @param row_count amount of contained values
 * @param result skew of the given vector
 * @return int error return code, always 0
 */
static int lsSkewUf(float *vector, float average, float variance, int row_count,
                    float *result) {
  *result = 0;
  for (int i = 0; i < row_count; i++) {
    *result += ((*(vector + i) - average) / variance) *
               ((*(vector + i) - average) / variance) *
               ((*(vector + i) - average) / variance);
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
  } else if (value0 == value1) {
    *result = 0;
  }
  return 0;
}

/**
 * @brief (float) Measures the distance of two values to 0.
 *
 * @param value0 first value
 * @param value1 second value
 * @param result 1 if the first value is further away from 0 then the second
 * value, otherwise 0
 * @return int error return code
 */
static int lsIsCloserToZerof(float value0, float value1, int *result) {
  value0 = value0 < 0 ? -value0 : value0;
  value1 = value1 < 0 ? -value1 : value1;
  if (value0 < value1) {
    *result = 0;
  } else if (value0 > value1) {
    *result = 1;
  } else if (value0 == value1) {
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
    printf("exception in lsAverage: ");
    return -1;
  }
  double sd;
  errnum = lsVariance(vector, average, row_count, &sd);
  if (errnum != 0) { // Standardabweichung
    printf("exception in lsVariance: ");
    return -2;
  }
  double new_skew;
  errnum = lsSkew(vector, average, sd, row_count, &new_skew);
  if (errnum != 0) { // Schiefe
    printf("exception in lsSkew: ");
    return -3;
  }
  int compare_flag;
  errnum = lsIsCloserToZero(*skew, new_skew, &compare_flag);
  if (errnum != 0) { // Vergleich mit vorheriger Schiefe
    printf("exception in lsIsCloserToZero: ");
    return -1;
  }
  if (compare_flag) {
    *skew = new_skew;
    *result = 1;
  }
  return 0;
}

/**
 * @brief (float) Calculates the skew of the given vector and compares if a
 * given skew is closer to zero.
 *
 * @param vector    containing all values
 * @param row_count amount of contained values
 * @param skew  given skew
 * @param result 1 if the new skew is closer to 0, 0 otherwise
 * @return int error return code
 */
static int lsSkewIntervalStepf(float *vector, int row_count, float *skew,
                               int *result) {
  *result = 0;
  float average;
  int errnum = lsAveragef(vector, row_count, &average);
  if (errnum != 0) { // Mittelwert
    printf("exception in lsAveragef: ");
    switch (errnum) {
    case -1:
      printf("Vector too small, not enough components\n");
      break;

    case -2:
      printf("positive overflow\n");
      break;

    case -3:
      printf("negative overflow\n");
      break;

    default:
      printf("unhandled exception\n");
      break;
    }
    return -1;
  }
  float sd;
  errnum = lsVariancef(vector, average, row_count, &sd);
  if (errnum != 0) { // Standardabweichung
    printf("exception in lsVariancef: ");
    switch (errnum) {
    case -1:
      printf("Vector too small, not enough components\n");
      break;

    case -2:
      printf("positive overflow\n");
      break;

    case -3:
      printf("negative overflow\n");
      break;

    default:
      printf("unhandled exception\n");
      break;
    }
    return -1;
  }
  float new_skew;
  errnum = lsSkewf(vector, average, sd, row_count, &new_skew);
  if (errnum != 0) { // Schiefe
    printf("exception in lsSkewf: ");
    switch (errnum) {
    case -1:
      printf("Vector too small, not enough components\n");
      break;

    case -2:
      printf("positive overflow\n");
      break;

    case -3:
      printf("negative overflow\n");
      break;

    case -4:
      printf("divided by zero\n");
      break;

    default:
      printf("unhandled exception\n");
      break;
    }
    return -1;
  }
  int compare_flag;
  errnum = lsIsCloserToZerof(*skew, new_skew, &compare_flag);
  if (errnum != 0) { // Vergleich mit vorheriger Schiefe
    printf("exception in lsIsCloserToZero: ");
    switch (errnum) {
    default:
      printf("unhandled exception\n");
      break;
    }
    return -1;
  }
  if (compare_flag) {
    *skew = new_skew;
    *result = 1;
  }
  return 0;
}

/**
 * @brief (double) Calculates the skew of the given vector and compares if a
 * given skew is closer to zero without intercepting any errors.
 *
 * @param vector    containing all values
 * @param row_count amount of contained values
 * @param skew  given skew
 * @param result 1 if the new skew is closer to 0, 0 otherwise
 * @return int error return code, always 0
 */
static int lsSkewIntervalStepU(double *vector, int row_count, double *skew,
                               int *result) {
  *result = 0;
  double average = 0;
  lsAverageU(vector, row_count, &average);
  double variance = 0;
  lsVarianceU(vector, average, row_count, &variance);
  double new_skew = 0;
  lsSkewU(vector, average, variance, row_count, &new_skew);
  int compare_flag = 0;
  lsIsCloserToZero(*skew, new_skew, &compare_flag);
  if (compare_flag) {
    *skew = new_skew;
    *result = 1;
  }
  return 0;
}

/**
 * @brief (float) Calculates the skew of the given vector and compares if a
 * given skew is closer to zero without intercepting any errors.
 *
 * @param vector    containing all values
 * @param row_count amount of contained values
 * @param skew  given skew
 * @param result 1 if the new skew is closer to 0, 0 otherwise
 * @return int error return code, always 0
 */
static int lsSkewIntervalStepUf(float *vector, int row_count, float *skew,
                                int *result) {
  *result = 0;
  float average = 0;
  lsAverageUf(vector, row_count, &average);
  float variance = 0;
  lsVarianceUf(vector, average, row_count, &variance);
  float new_skew = 0;
  lsSkewUf(vector, average, variance, row_count, &new_skew);
  int compare_flag = 0;
  lsIsCloserToZerof(*skew, new_skew, &compare_flag);
  if (compare_flag) {
    *skew = new_skew;
    *result = 1;
  }
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
                   double *result_skew) {
  *result_skew = g_maxHighDouble;
  double *zws = (double *)malloc(sizeof(double) * row_count);
  if (zws == NULL) {
    printf("Error: failed to allocate memory.\n");
    return -1;
  }
  memset(zws, 0, sizeof(double) * row_count);
  *result_lambda = interval_start;
  buildBoundaryBox(interval_end, interval_start);
  for (double lambda_i = interval_start; lambda_i <= interval_end;
       lambda_i += interval_step) {
    for (int i = 0; i < row_count; i++) {
      if (yjCalculation(*(vector + i), lambda_i, zws + i) != 0) {
        printf("error occured during yeoJohnson\n");
        free(zws);
        return -2;
      }
    }
    int skew_test_flag;
    if (lsSkewIntervalStep(zws, row_count, result_skew, &skew_test_flag) != 0) {
      printf("error occured during skewTest\n");
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
 * @brief (float) Searching a lambda resulting in the skew closest to zero.
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
int lsLambdaSearchf(float *vector, float interval_start, float interval_end,
                    float interval_step, int row_count, float *result_lambda,
                    float *result_skew) {
  float *zws = (float *)malloc(sizeof(float) * row_count);
  if (zws == NULL) {
    printf("Error: failed to allocate memory.\n");
    return -1;
  }
  memset(zws, 0, sizeof(float) * row_count);
  *result_lambda = interval_start;
  buildBoundaryBoxf(interval_start, interval_end);
  for (float lambda_i = interval_start; lambda_i <= interval_end;
       lambda_i += interval_step) {
    for (int i = 0; i < row_count; i++) {
      if (yjCalculationf(*(vector + i), lambda_i, &zws[i]) != 0) {
        printf("error occured during yeoJohnson\n");
        free(zws);
        return -1;
      }
    }
    int skew_test_flag;
    if (lsSkewIntervalStepf(zws, row_count, result_skew, &skew_test_flag) !=
        0) {
      printf("error occured during skewTest\n");
      free(zws);
      return -2;
    }
    if (skew_test_flag) {
      *result_lambda = lambda_i;
    }
  }
  free(zws);
  return 0;
}

/**
 * @brief (double) Searching a lambda resulting in the skew closest to zero
 *          without intercepting any errors.
 *
 * @param vector containing all values
 * @param interval_start start value of the search
 * @param interval_end end value of the search
 * @param interval_step steps inside the interval
 * @param row_count amount of contained values
 * @param result_lambda lambda resulting in the skew closest to zero
 * @param result_skew skew which can be achieved with result_lambda
 * @return int error return code, always 0
 */
int lsLambdaSearchU(double *vector, double interval_start, double interval_end,
                    double interval_step, int row_count, double *result_lambda,
                    double *result_skew) {
  double *zws = (double *)malloc(sizeof(double) * row_count);
  if (zws == NULL) {
    printf("Error: failed to allocate memory.\n");
    return -1;
  }
  memset(zws, 0, sizeof(double) * row_count);
  *result_lambda = interval_start;
  for (double lambda_i = interval_start; lambda_i <= interval_end;
       lambda_i += interval_step) {
    for (int i = 0; i < row_count; i++) {
      yjCalculationU(*(vector + i), lambda_i, &zws[i]);
    }
    int skew_test_flag;
    lsSkewIntervalStepU(vector, row_count, result_skew, &skew_test_flag);
    if (skew_test_flag) {
      *result_lambda = lambda_i;
    }
  }
  free(zws);
  return 0;
}

/**
 * @brief (float) Searching a lambda resulting in the skew closest to zero
 *          without intercepting any errors.
 *
 * @param vector containing all values
 * @param interval_start start value of the search
 * @param interval_end end value of the search
 * @param interval_step steps inside the interval
 * @param row_count amount of contained values
 * @param result_lambda lambda resulting in the skew closest to zero
 * @param result_skew skew which can be achieved with result_lambda
 * @return int error return code, always 0
 */
int lsLambdaSearchUf(float *vector, float interval_start, float interval_end,
                     float interval_step, int row_count, float *result_lambda,
                     float *result_skew) {
  float *zws = (float *)malloc(sizeof(float) * row_count);
  if (zws == NULL) {
    printf("Error: failed to allocate memory.\n");
    return -1;
  }
  memset(zws, 0, sizeof(float) * row_count);
  *result_lambda = interval_start;
  for (float lambda_i = interval_start; lambda_i <= interval_end;
       lambda_i += interval_step) {
    // Berechnen des transformierten Vektors
    for (int i = 0; i < row_count; i++) {
      yjCalculationUf(*(vector + i), lambda_i, &zws[i]);
    }
    // Überprüfung der Schiefe
    int skew_test_flag;
    lsSkewIntervalStepUf(vector, row_count, result_skew, &skew_test_flag);
    if (skew_test_flag) {
      *result_lambda = lambda_i;
    }
  }
  free(zws);
  return 0;
}

/*****************************************************************************
 *                               TESTS
 *****************************************************************************/

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
  double result = 0;
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
  float result = 0;
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
  double result = 0;
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
  float result = 0;
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