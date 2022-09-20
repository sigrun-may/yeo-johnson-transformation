/*
 *   lambdaSearch.h
 *
 *   created on: 05.09.2022
 *   author: jbrenig
 *
 */

#ifndef LAMBDASEARCH_H
#define LAMBDASEARCH_H

int lsLambdaSearch(double *vector, double interval_start, double interval_end,
                   double interval_step, int row_count, double *result_lambda,
                   double *result_skew);
int lsLambdaSearchf(float *vector, float interval_start, float interval_end,
                    float interval_step, int row_count, float *result_lambda,
                    float *result_skew);
int lsLambdaSearchU(double *vector, double interval_start, double interval_end,
                    double interval_step, int row_count, double *result_lambda,
                    double *result_skew);
int lsLambdaSearchUf(float *vector, float interval_start, float interval_end,
                     float interval_step, int row_count, float *result_lambda,
                     float *result_skew);

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

#endif /* LAMBDASEARCH_H */