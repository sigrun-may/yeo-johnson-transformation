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

int ciLambdaOperationOnMatrixFromFileS(char *file_path, matrix *vector_field,
                                       double interval_start,
                                       double interval_end,
                                       double interval_step);

#endif /* COMINTERFACE_H */