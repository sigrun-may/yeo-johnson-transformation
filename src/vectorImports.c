/*H***************************************************************************
 * FILENAME :   vectorImports.c
 *
 * DESCRIPTION  :
 *          Utilities to import vector data from .csv and python dataframe.
 *
 * PUBLIC FUNCTIONS :
 *
 *
 * NOTES    :
 *
 *
 * AUTHOR   :       jbrenig           START DATE    : 07 September 2022
 *
 * CHANGES  :
 *
 * DATE     WHO     DETAIL
 *
 *H*/

/*****************************************************************************
 *                               INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/testFramework.h"
#include "include/vectorImports.h"

/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

/**
 * @brief Get the Matrix Size From Csv object, last row of csv needs to end with EOL
 *
 * @param file csv to be read and interpreted
 * @param column_size resulting amount of columns
 * @param row_size resulting amount of rows
 * @return int error return code
 */
static int getMatrixSizeFromCsv(FILE *file, int *column_size, int *row_size) {
  *row_size = 0;
  *column_size = 0;
  if (file == NULL) {
    printf("\tfile passed is null.\n");
    return -1;
  }
  int next_char = fgetc(file);
  int column_counter = 0;
  while ((next_char = fgetc(file)) != EOF) {
    if (next_char == DOT) {
      printf("\tWrong format, pls use \',\' as decimal point and \';\' as "
             "seperator\n");
      return -2;
    }
    if (next_char == SEMICOLON) { // next_char == KOMMA ||
      column_counter++;
    } else if (next_char == EOL) {
      (*row_size)++;
      column_counter++;
      if (column_counter > *column_size) {
        *column_size = column_counter;
      }
      column_counter = 0;
    }
  }
  rewind(file);
  return 0;
}

/**
 * @brief (double) fill matrix with data from csv file
 *
 * @param file containing data to be filled int matrix struct
 * @param matrix destination for data first pointer on columns
 * @param row_count amount of rows in matrix
 * @param column_count amount of columns in matrix
 * @return int error return code
 */
static int fillMatrixFromCsv(FILE *file, double **matrix, int row_count,
                             int column_count) {
  char *line = (char *)malloc(MAX_STRING_SIZE);
  if (line == NULL) {
    printf("\tMemory could not be allocated\n");
    return -1;
  }
  if (file == NULL) {
    printf("\tFile is null\n");
    return -2;
  }
  if (matrix == NULL) {
    printf("\tMatrix is null\n");
    return -3;
  }
  if (row_count == 0) {
    printf("\trow_count is 0, no rows to be read\n");
    return -4;
  }
  if (column_count == 0) {
    printf("\tcolumn_count is 0, no columns to be read\n");
    return -5;
  }
  int row_counter = 0;
  while (fgets(line, MAX_STRING_SIZE, file)) {
    const char *tok = strtok(line, ";");
    for (int i = 0; i < column_count; i++) {
      double parameter;
      if (tok != NULL) {
        parameter = strtod(tok, NULL);
        *(*(matrix + i) + row_counter) = parameter;
      }
      tok = strtok(NULL, ";\n");
    }
    row_counter++;
  }
  free(line);
  return 0;
}

/**
 * @brief (float) fill matrix with data from csv file
 *
 * @param file containing data to be filled int matrix struct
 * @param matrix destination for data first pointer on columns
 * @param row_count amount of rows in matrix
 * @param column_count amount of columns in matrix
 * @return int error return code
 */
static int fillMatrixFromCsvf(FILE *file, float **matrix, int row_count,
                              int column_count) {
  char *line = (char *)malloc(MAX_STRING_SIZE);
  if (line == NULL) {
    printf("\tMemory could not be allocated\n");
    return -1;
  }
  if (file == NULL) {
    printf("\tFile is null\n");
    return -2;
  }
  if (matrix == NULL) {
    printf("\tMatrix is null\n");
    return -3;
  }
  if (row_count == 0) {
    printf("\trow_count is 0, no rows to be read\n");
    return -4;
  }
  if (column_count == 0) {
    printf("\tcolumn_count is 0, no columns to be read\n");
    return -5;
  }
  int row_counter = 0;
  while (fgets(line, MAX_STRING_SIZE, file)) {
    const char *tok = strtok(line, ";");
    for (int i = 0; i < column_count; i++) {
      double parameter;
      if (tok != NULL) {
        parameter = strtof(tok, NULL);
        *(*(matrix + row_counter) + i) = parameter;
      }
      tok = strtok(NULL, ";\n");
    }
    row_counter++;
  }
  free(line);
  return 0;
}

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief (double) Import data contained in file stored at file_path into matrix
 * struct
 *
 * @param file_path file origin
 * @param vector_list matrix struct, data destination
 * @return int error return code
 */
int importVectorTableFromCsv(char *file_path, matrix *vector_list) {
  int errnum;
  FILE *file;
  if (file_path == NULL) {
    printf("\tfile_path is null\n");
    return -1;
  }
  if (vector_list == NULL) {
    printf("\tvector_list is null\n");
    return -2;
  }
  file = fopen(file_path, "r");
  if (file == NULL) {
    printf("\tFile does not exist in data directory.\n");
    return -3;
  }
  errnum = getMatrixSizeFromCsv(file, &vector_list->cols, &vector_list->rows);
  if (errnum != 0) {
    printf("\texception while getting dimensions.\n");
    return -4;
  }
  {
    double **column_addresses =
        (double **)malloc(sizeof(double **) * vector_list->cols);
    if (column_addresses == NULL) {
      printf("\tFailed to allocate memory for column_addresses.\n");
      return -5;
    }
    for (int i = 0; i < vector_list->cols; i++) {
      double *row_info = (double *)malloc(sizeof(double) * vector_list->rows);
      if (row_info == NULL) {
        printf("\tFailed to allocate memory for row_info.\n");
        return -6;
      }
      *(column_addresses + i) = row_info;
    }
    vector_list->data = column_addresses;
    double *new_mem = (double *)malloc(sizeof(double) * vector_list->cols);
    if (new_mem == NULL) {
      printf("\tFailed to allocate memory for lambda_storage.\n");
      return -7;
    }
    vector_list->lambda = new_mem;
    new_mem = (double *)malloc(sizeof(double) * vector_list->cols);
    if (new_mem == NULL) {
      printf("\tFailed to allocate memory for skew_storage.\n");
      return -8;
    }
    vector_list->skew = new_mem;
  }
  if (fillMatrixFromCsv(file, vector_list->data, vector_list->rows,
                        vector_list->cols) != 0) {
    printf("\texception while parsing csv file.\n");
    return -9;
  }
  return 0;
}

/**
 * @brief (float) Import data contained in file stored at file_path into matrix
 * struct
 *
 * @param file_path file origin
 * @param vector_list matrixf struct, data destination
 * @return int error return code
 */
int importVectorTableFromCsvf(char *file_path, matrixf *vector_list) {
  int errnum;
  FILE *file;
  file = fopen(file_path, "r");
  if (file == NULL) {
    printf("\tFile does not exist in data directory.\n");
    return -1;
  }
  errnum = getMatrixSizeFromCsv(file, &vector_list->rows, &vector_list->cols);
  if (errnum != 0) {
    printf("\texception while reading file.\n");
    return -2;
  }
  {
    float **column_addresses =
        (float **)malloc(sizeof(float **) * vector_list->cols);
    if (column_addresses == NULL) {
      printf("\tFailed to allocate memory.\n");
      return -3;
    }
    for (int i = 0; i < vector_list->cols; i++) {
      float *row_info = (float *)malloc(sizeof(float) * vector_list->rows);
      if (row_info == NULL) {
        printf("\tFailed to allocate memory for row_info.\n");
        return -3;
      }
      *(column_addresses + i) = row_info;
    }
    vector_list->data = column_addresses;
    float *new_mem = (float *)malloc(sizeof(float) * vector_list->cols);
    if (new_mem == NULL) {
      printf("\tFailed to allocate memory for lambda_storage.\n");
      return -3;
    }
    vector_list->lambda = new_mem;
    new_mem = (float *)malloc(sizeof(float) * vector_list->cols);
    if (new_mem == NULL) {
      printf("\tFailed to allocate memory for skew_storage.\n");
      return -3;
    }
    vector_list->skew = new_mem;
  }
  if (fillMatrixFromCsvf(file, vector_list->data, vector_list->rows,
                         vector_list->cols) != 0) {
    printf("\texception while parsing csv file.\n");
    return -5;
  }
  return 0;
}

/*****************************************************************************
 *                                  TESTS
 *****************************************************************************/

void test_getMatrixSizeFromCsv(void) {
  printf("Testing getMatrixSizeFromCsv in vectorImports.c\n");
  int column_size = 0;
  int row_size = 0;
  FILE *wrong_file = fopen(
      "./data/wrong.csv",
      "r");
  assert_int_equals(getMatrixSizeFromCsv(NULL, &column_size, &row_size), -1,
                    "Error: file is null but still opened");
  assert_int_equals(getMatrixSizeFromCsv(wrong_file, &column_size, &row_size),
                    -2, "Error: wrong format but was read");
  fclose(wrong_file);
  FILE *file = fopen(
      "./data/Mappe2.CSV",
      "r");
  fpos_t file_pointer;
  fgetpos(file, &file_pointer);
  assert_int_equals(getMatrixSizeFromCsv(file, &column_size, &row_size), 0,
                    "Error: file exists but is null");
  fpos_t after_pointer;
  fgetpos(file, &after_pointer);
  assert_int_equals(column_size, 62,
                    "Error: column_size is not equal to columns in document");
  assert_int_equals(row_size, 2000,
                    "Error: row_size is not equal to rows in document");
  assert_double_equals((double)after_pointer, (double)file_pointer,
                       "Error: File pointer is not reset");
  fclose(file);
  file = fopen(
      "./data/data1.CSV",
      "r");
  getMatrixSizeFromCsv(file, &column_size, &row_size);
  assert_int_equals(column_size, 1, "Error: column_size should be 1");
  assert_int_equals(row_size, 4, "Error: row_size should be 4");
  fclose(file);
  printf("...done\n");
}

void test_fillMatrixFromCsv(void) {
  printf("Testing fillMatrixFromCsv in vectorImports.c\n");
  matrix test;
  FILE *file = fopen(
      "./data/data1.CSV",
      "r");
  assert_int_equals(fillMatrixFromCsv(NULL, test.data, 1, 1), -2,
                    "Error: File is null, should be aborted");
  assert_int_equals(fillMatrixFromCsv(file, NULL, 1, 1), -3,
                    "Error: matrix is null, should be aborted");
  assert_int_equals(fillMatrixFromCsv(file, test.data, 0, 1), -4,
                    "Error: row_count is 0, should be aborted");
  assert_int_equals(fillMatrixFromCsv(file, test.data, 4, 1), 0,
                    "Error: all parameters are correct, should be executed");
  fillMatrixFromCsv(file, test.data, 4, 1);
  assert_double_equals(*(*(test.data) + 0), (double)-5,
                       "Error: 1st number in matrix is not as expected");
  assert_double_equals(*(*(test.data) + 1), (double)-3,
                       "Error: 2nd number in matrix is not as expected");
  assert_double_equals(*(*(test.data) + 2), (double)-3,
                       "Error: 3rd number in matrix is not as expected");
  assert_double_equals(*(*(test.data) + 3), (double)-2,
                       "Error: 4th number in matrix is not as expected");
  printf("...done\n");
}

void test_fillMatrixFromCsvf(void) {
  printf("Testing getMatrixSizeFromCsvf in vectorImports.c\n");
  matrixf test;
  FILE *file = fopen(
      "./data/data1.CSV",
      "r");
  assert_int_equals(fillMatrixFromCsvf(NULL, test.data, 1, 1), -2,
                    "Error: File is null, should be aborted");
  assert_int_equals(fillMatrixFromCsvf(file, NULL, 1, 1), -3,
                    "Error: matrix is null, should be aborted");
  assert_int_equals(fillMatrixFromCsvf(file, test.data, 0, 1), -4,
                    "Error: row_count is 0, should be aborted");
  assert_int_equals(fillMatrixFromCsvf(file, test.data, 4, 1), 0,
                    "Error: all parameters are correct, should be executed");
  fillMatrixFromCsvf(file, test.data, 4, 1);
  assert_double_equals(*(*(test.data) + 0), (float)-5,
                       "Error: 1st number in matrix is not as expected");
  assert_double_equals(*(*(test.data) + 1), (float)-3,
                       "Error: 2nd number in matrix is not as expected");
  assert_double_equals(*(*(test.data) + 2), (float)-3,
                       "Error: 3rd number in matrix is not as expected");
  assert_double_equals(*(*(test.data) + 3), (float)-2,
                       "Error: 4th number in matrix is not as expected");
  printf("...done\n");
}

void test_importVectorTableFromCsv(void) {
  printf("Testing importVectorTableFromCsv in vectorImports.c\n");
  char *file_path =
      "./data/data2.CSV";
  char *wrong_file_path =
      "./data/wrong.csv";
  matrix vector_list;
  assert_int_equals(importVectorTableFromCsv(NULL, &vector_list), -1,
                    "Error: file path is null, should abort");
  assert_int_equals(importVectorTableFromCsv(file_path, NULL), -2,
                    "Error: vector_list is null, should abort");
  assert_int_equals(importVectorTableFromCsv("test/path.csv", &vector_list), -3,
                    "Error: file does not exist, should abort");
  assert_int_equals(importVectorTableFromCsv(wrong_file_path, &vector_list), -4,
                    "Error: dimensions would not be determined, should abort");
  assert_int_equals(importVectorTableFromCsv(file_path, &vector_list), 0,
                    "Error: should execute but did not");
  assert_not_null((void *)vector_list.data,
                  "Error: No memory allocated for data");
  for (int i = 0; i < vector_list.cols; i++) {
    assert_not_null((void *)*(vector_list.data + i),
                    "Error: no memory allocated for data");
  }
  assert_not_null((void *)vector_list.lambda,
                  "Error: No memory allocated for lambda");
  assert_not_null((void *)vector_list.skew,
                  "Error: No memory allocated for skew");
  char *wrong_file_path2 =
      "./data/wrong2.csv";
  assert_int_equals(importVectorTableFromCsv(wrong_file_path2, &vector_list),
                    -9, "Error: cols and rows are 0, should abort");
  printf("...done\n");
}

void test_importVectorTableFromCsvf(void) {
  printf("Testing importVectorTableFromCsvf in vectorImports.c\n");
  char *file_path =
      "./data/data2.CSV";
  char *wrong_file_path =
      "./data/wrong.csv";
  matrixf vector_list;
  assert_int_equals(importVectorTableFromCsvf(NULL, &vector_list), -1,
                    "Error: file path is null, should abort");
  assert_int_equals(importVectorTableFromCsvf(file_path, NULL), -2,
                    "Error: vector_list is null, should abort");
  assert_int_equals(importVectorTableFromCsvf("test/path.csv", &vector_list),
                    -3, "Error: file does not exist, should abort");
  assert_int_equals(importVectorTableFromCsvf(wrong_file_path, &vector_list),
                    -4,
                    "Error: dimensions would not be determined, should abort");
  assert_int_equals(importVectorTableFromCsvf(file_path, &vector_list), 0,
                    "Error: should execute but did not");
  assert_not_null((void *)vector_list.data,
                  "Error: No memory allocated for data");
  for (int i = 0; i < vector_list.cols; i++) {
    assert_not_null((void *)*(vector_list.data + i),
                    "Error: no memory allocated for data");
  }
  assert_not_null((void *)vector_list.lambda,
                  "Error: No memory allocated for lambda");
  assert_not_null((void *)vector_list.skew,
                  "Error: No memory allocated for skew");
  char *wrong_file_path2 =
      "./data/wrong2.csv";
  assert_int_equals(importVectorTableFromCsvf(wrong_file_path2, &vector_list),
                    -9, "Error: cols and rows are 0, should abort");
  printf("...done\n");
}