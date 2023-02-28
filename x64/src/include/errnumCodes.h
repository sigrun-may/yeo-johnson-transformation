/****************************************************************
 * Copyright (c) 2023 Jerome Brenig, Sigrun May
 * Ostfalia Hochschule für angewandte Wissenschaften
 *
 * This software is distributed under the terms of the MIT license
 * which is available at https://opensource.org/licenses/MIT
 *
 *   errnumCodes.h
 */

#ifndef COMINTERFACE_H
#define COMINTERFACE_H

#define ERR_LAMBDA_SEARCH 0x1000 // error on lambda search
#define ERR_TRANSFORM 0x2000     // error on transformation

#define ERR_SKEW_TEST 0x0100         // error in lambdaSearch.c skewIntervalStep
#define ERR_ABORT_YEO_JOHNSON 0x0200 // error occured in yeoJohnson.c
#define ERR_FAILED_ALLOCATE_MEMORY 0x0300 // failed to allocate memory

#define ERR_YJ1_ID 0x0010         // error in yeoJohnson formular 1
#define ERR_YJ2_ID 0x0020         // error in yeoJohnson formular 2
#define ERR_YJ3_ID 0x0030         // error in yeoJohnson formular 3
#define ERR_YJ4_ID 0x0040         // error in yeoJohnson formular 4
#define ERR_AVERAGE 0x0050        // error in lambdaSearch average
#define ERR_DEVIATION 0x0060      // error in lambdaSearch variance
#define ERR_SKEW 0x0070           // error in lambdaSearch skew
#define ERR_CLOSER_TO_ZERO 0x0080 // error in lambdaSearch isCloserToZero

#define ERR_VECTOR_IS_NULL 0x0001  // the pointer is null
#define ERR_NOT_ENOUGH_ROWS 0x0002 // not enough rows to calculate avg/sd/skew
#define ERR_VALUE_OVERFLOW 0x0003  // input value would overflow
#define ERR_VALUE_NOT_IN_BB 0x0004 // input value with lambda would overflow
#define ERR_BB_NOT_SET 0x0005      // boundary box could not be set

#endif