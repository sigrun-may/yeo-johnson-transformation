/*
 *   vectorImports.h
 *
 *   created on 07.09.2022
 *   author: jbrenig
 *
 */

#ifndef VECTORIMPORTS_H
#define VECTORIMPORTS_H

// defines
#define KOMMA 44
#define DOT 46
#define SEMICOLON 59
#define EOL 10
#define MAX_STRING_SIZE 1024

// structs
typedef struct _MATRIX {
  int rows;
  int cols;
  double **data;
  double *lambda;
  double *skew;
} MATRIX;

// public functions
int importVectorTableFromCsv(char *file_path, MATRIX **vector);

// unit tests
#ifdef UNIT_TEST
void test_getMatrixSizeFromCsv(void);
void test_fillMatrixFromCsv(void);
void test_fillMatrixFromCsvf(void);
void test_importVectorTableFromCsv(void);
void test_importVectorTableFromCsvf(void);
#endif

#endif /* VECTORIMPORTS_H */