/****************************************************************
* Copyright (c) 2023 Jerome Brenig, Sigrun May
* Ostfalia Hochschule für angewandte Wissenschaften
*
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
*
 *   comInterface.h
 */

#ifndef COMINTERFACE_H
#define COMINTERFACE_H

#include "vectorImports.h"

#define BOOL int

// public functions
int ciLambdaOperationOnMatrixFromFileS(char *file_path, double interval_start,
                                       double interval_end,
                                       double interval_step,
                                       MATRIX **vector_field, BOOL standardize,
                                       BOOL time_stamps);

int ciLambdaOperation(double interval_start, double interval_end,
                      double interval_step, MATRIX *input_matrix,
                      BOOL standardize, BOOL time_stamps);

int ciSmartOperation(double interval_start, double interval_end, int precision,
                     MATRIX *input_matrix, BOOL standardize, BOOL time_stamps);

int ciParallelOperation(double interval_start, double interval_end,
                        int precision, MATRIX *input_matrix, BOOL standardize,
                        BOOL time_stamps, int thread_count);

#endif /* COMINTERFACE_H */