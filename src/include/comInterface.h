/*
 *   comInterface.h
 *
 *   created on: 14.09.2022
 *   author: jbrenig
 *
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

#endif /* COMINTERFACE_H */