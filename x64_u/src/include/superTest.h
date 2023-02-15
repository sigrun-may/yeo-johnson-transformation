/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
*
 *   superTest.h
 */

#ifndef SUPERTEST_H
#define SUPERTEST_H

// public functions
#ifdef UNIT_TEST
void test_super_yj(void);
void test_super_vi(void);
void test_super_ls(void);
#endif

#endif /* LAMBDASEARCH_H */