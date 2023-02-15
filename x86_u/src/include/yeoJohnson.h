/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
* /

/*
 *   yeoJohnson.h
 */

#ifndef YEOJOHNSON_H
#define YEOJOHNSON_H

#define _USE_MATH_DEFINES

typedef struct _BOUNDARYBOXF {
  double upper_limit;
  double lower_limit;
} BOUNDARYBOXF;

// public functions
void buildBoundaryBox(float lower_lambda, float upper_lambda);
int yjCalculation(float y, float lambda, float *result);

int yjTransformBy(float **vector, float lambda, int rows);

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