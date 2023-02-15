/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
*
 *   lambdaSearch.h
 */

#ifndef LAMBDASEARCH_H
#define LAMBDASEARCH_H

// public functions
int lsVariance(double *vector, double average, int row_count, double *result);
int lsAverage(double *vector, int row_count, double *result);

int lsLambdaSearch(double *vector, double interval_start, double interval_end,
                   double interval_step, int row_count, double *result_lambda,
                   double *result_skew);
int lsSmartSearch(double *vector, double interval_start, double interval_end,
                  int precision, int row_count, double *result_lambda,
                  double *result_skew);

// unit tests
#ifdef UNIT_TEST
void test_lsAverage(void);
void test_lsAveragef(void);
void test_lsAverageU(void);
void test_lsAverageUf(void);
void test_lsVariance(void);
void test_lsVariancef(void);
void test_lsVarianceU(void);
void test_lsVarianceUf(void);
void test_lsSkew(void);
void test_lsSkewf(void);
void test_lsSkewU(void);
void test_lsSkewUf(void);
void test_lsIsCloserToZero(void);
void test_lsIsCloserToZerof(void);
void test_lsSkewIntervalStep(void);
void test_lsSkewIntervalStepf(void);
void test_lsSkewIntervalStepU(void);
void test_lsSkewIntervalStepUf(void);
void test_lsLambdaSearch(void);
void test_lsLambdaSearchf(void);
void test_lsLambdaSearchU(void);
void test_lsLambdaSearchUf(void);
#endif

#endif /* LAMBDASEARCH_H */