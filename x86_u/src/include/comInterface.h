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
int ciLambdaOperationOnMatrixFromFileS(char *file_path, float interval_start,
                                       float interval_end,
                                       float interval_step,
                                       MATRIXF **vector_field, BOOL standardize,
                                       BOOL time_stamps);

int ciLambdaOperation(float interval_start, float interval_end,
                      float interval_step, MATRIXF *input_matrix,
                      BOOL standardize, BOOL time_stamps);

int ciSmartOperation(float interval_start, float interval_end, int precision,
                     MATRIXF *input_matrix, BOOL standardize, BOOL time_stamps);

int ciParallelOperation(float interval_start, float interval_end,
                        int precision, MATRIXF *input_matrix, BOOL standardize,
                        BOOL time_stamps, int thread_count);

#endif /* COMINTERFACE_H */