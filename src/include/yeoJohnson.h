/*
 *   yeoJohnson.h
 *
 *   created on 05.09.2022
 *   author: jbrenig
 *
 */

#ifndef YEOJOHNSON_H
#define YEOJOHNSON_H

#define _USE_MATH_DEFINES

typedef struct {
  double upper_limit;
  double lower_limit;
} boundaryBox;

typedef struct {
  double upper_limit;
  double lower_limit;
} boundaryBoxf;

// public functions
void buildBoundaryBox(double lower_lambda, double upper_lambda);
void buildBoundaryBoxf(float lower_lambda, float upper_lambda);
int yjCalculation(double y, double lambda, double *result);
int yjCalculationf(float y, float lambda, float *result);
int yjCalculationU(double y, double lambda, double *result);
int yjCalculationUf(float y, float lambda, float *result);

// unit tests
#ifdef UNIT_TEST
void test_yj1(void);
void test_yj1f(void);
void test_yj1U(void);
void test_yj1Uf(void);

void test_yj2(void);
void test_yj2f(void);
void test_yj2U(void);
void test_yj2Uf(void);

void test_yj3(void);
void test_yj3f(void);
void test_yj3U(void);
void test_yj3Uf(void);

void test_yj4(void);
void test_yj4f(void);
void test_yj4U(void);
void test_yj4Uf(void);

void test_buildBoundaryBox(void);

void test_yjCalculation(void);
void test_yjCalculationf(void);
void test_yjCalculationU(void);
void test_yjCalculationUf(void);
#endif

#endif /* YEOJOHNSON_H */