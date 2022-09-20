/*
 *   testFramework.h
 *
 *   created on 12.09.2022
 *   author: jbrenig
 *
 */

#ifndef TESTFRAMEWORK_H
#define TESTFRAMEWORK_H

int assert_int_equals(int a, int b, char *desc);
int assert_double_equals(double a, double b, char *desc);
int assert_float_equals(float a, float b, char *desc);
int assert_string_equals(char *a, char *b, char *desc);
int is_in_bound(double a, double origin, double range, char *desc);
int assert_not_null(void *ptr, char *desc);

#endif /* TESTFRAMEWORK_H */