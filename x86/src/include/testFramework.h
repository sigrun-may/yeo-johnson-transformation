/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
*
 *   testFramework.h
 */

#ifndef TESTFRAMEWORK_H
#define TESTFRAMEWORK_H

// public functions
int assert_int_equals(int a, int b, char *desc);
int assert_double_equals(double a, double b, char *desc);
int assert_float_equals(float a, float b, char *desc);
int assert_string_equals(char *a, char *b, char *desc);
int is_in_bound(double a, double origin, double range, char *desc);
int assert_not_null(void *ptr, char *desc);

#endif /* TESTFRAMEWORK_H */