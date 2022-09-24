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

// public functions
int ciLambdaOperationOnMatrixFromFileS(char *file_path, double interval_start,
                                       double interval_end,
                                       double interval_step,
                                       MATRIX **vector_field);

int ciLambdaOperation(double interval_start, double interval_end, double interval_step, MATRIX *input_matrix);

#endif /* COMINTERFACE_H */