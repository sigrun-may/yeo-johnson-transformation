/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
*
 * FILENAME : testFramework.c
 *
 * DESCRIBTION  :
 * functions for assert statements
 *
 * PUBLIC FUNCTIONS :
 * int assert_int_equals(int a, int b, char *desc)
 * int assert_double_equals(double a, double b, char *desc)
 * int assert_float_equals(float a, float b, char *desc)
 * int assert_string_equals(char *a, char *b, char *desc)
 * int is_in_bound(double a, double origin, double range, char *desc)
 * int assert_not_null(void *ptr, char *desc)
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

/*****************************************************************************
 *                           TEST FUNCTIONS
 *****************************************************************************/

/**
 * @brief checks if a is equal to b, prints a describtion if not
 *
 * @param a first integer
 * @param b second integer
 * @param desc describtion to be printed
 * @return int error return code
 */
int assert_int_equals(int a, int b, char *desc) {
    if (a == b) {
        return 0;
    } else {
        printf("%s\n", desc);
    }
    return -1;
}

/**
 * @brief checks if a is equal to b, prints a describtion if not
 *
 * @param a first integer
 * @param b second integer
 * @param desc describtion to be printed
 * @return int error return code
 */
int assert_double_equals(double a, double b, char *desc) {
    if (a == b) {
        return 0;
    } else {
        printf("%s\n", desc);
    }
    return -1;
}

/**
 * @brief checks if a is equal to b, prints a describtion if not
 *
 * @param a first integer
 * @param b second integer
 * @param desc describtion to be printed
 * @return int error return code
 */
int assert_float_equals(float a, float b, char *desc) {
    if (a == b) {
        return 0;
    } else {
        printf("%s\n", desc);
    }
    return -1;
}

/**
 * @brief checks if a is equal to b, prints a describtion if not
 *
 * @param a first integer
 * @param b second integer
 * @param desc describtion to be printed
 * @return int error return code
 */
int assert_string_equals(char *a, char *b, char *desc) {
    if (strcmp(a, b) == 0) {
        return 0;
    }
    printf("%s\n", desc);
    return -1;
}

/**
 * @brief checks if a is a given range around the origin
 *
 * @param a value
 * @param origin center point
 * @param range radius around origin
 * @param desc describtion to be printed
 * @return int error return code
 */
int is_in_bound(double a, double origin, double range, char *desc) {
    if (a < origin + range && a > origin - range) {
        return 0;
    }
    printf("%s\n", desc);
    return -1;
}

/**
 * @brief checks if a pointer is NOT null
 *
 * @param ptr pointer
 * @param desc decribtion
 * @return int
 */
int assert_not_null(void *ptr, char *desc) {
    if (ptr != NULL) {
        return 0;
    }
    printf("%s\n", desc);
    return -1;
}
