# Copyright (c) 2023 Jerome Brenig, Sigrun May, Ostfalia Hochschule für angewandte Wissenschaften
# This software is distributed under the terms of the MIT license
# which is available at https://opensource.org/licenses/MIT

import math
from collections import namedtuple
from ctypes import CDLL, POINTER, Structure, c_double, c_int, pointer

import numpy as np


Result = namedtuple(
    "Result", "unlabeled_transformed_data_np lambdas skews error_codes"
)


class _IntermediateResults(Structure):
    _fields_ = [
        ("rows", c_int),
        ("cols", c_int),
        ("data_matrix", POINTER(POINTER(c_double))),
        ("lambdas", POINTER(c_double)),
        ("skews", POINTER(c_double)),
        ("error_codes", POINTER(c_int)),
    ]


def _construct_c_matrix(matrix, c_data_type):
    #  getting dimensions
    column_dimension = np.size(matrix, 1)
    row_dimension = np.size(matrix, 0)
    #  initialising result vectors
    ptr_lambdas = (c_data_type * column_dimension)()
    ptr_skews = (c_data_type * column_dimension)()
    ptr_error_codes = (c_int * column_dimension)()
    #  initialising pointer array
    data_matrix = (POINTER(c_data_type) * column_dimension)()
    #  filling pointer array with values from unlabeled_data_np
    for i in range(column_dimension):
        data_row = (c_data_type * row_dimension)()
        for j in range(row_dimension):
            value = matrix[j][i]
            data_row[j] = c_data_type(value)
        data_matrix[i] = data_row
    #  transforming unlabeled_data_np into c format
    return _IntermediateResults(
        row_dimension,
        column_dimension,
        data_matrix,
        ptr_lambdas,
        ptr_skews,
        ptr_error_codes,
    )


def yeo_johnson_power_transformation(
    path_to_c_library: str,
    unlabeled_data_np: np.ndarray,
    interval_start: float = -3,
    interval_end: float = 3,
    interval_parameter: int = 14,
    standardize: bool = True,
    time_stamps: bool = False,
    number_of_threads: int = 1,
):
    yeo_johnson_c = CDLL(path_to_c_library).ciParallelOperation

    # defining parameters
    yeo_johnson_c.argtypes = [
        c_double,
        c_double,
        c_int,
        POINTER(_IntermediateResults),
        c_int,
        c_int,
        c_int,
    ]
    # defining return type
    yeo_johnson_c.restype = c_int

    # Returning unlabeled_data_np with adjusted values
    # constructing c struct
    temp_matrix = _construct_c_matrix(unlabeled_data_np, c_double)

    assert number_of_threads >= 1
    # call C function
    yeo_johnson_c(
        c_double(interval_start),
        c_double(interval_end),
        c_int(interval_parameter),
        pointer(temp_matrix),
        c_int(standardize),
        c_int(time_stamps),
        c_int(number_of_threads),
    )
    assert temp_matrix.cols == unlabeled_data_np.shape[1]
    assert temp_matrix.rows == unlabeled_data_np.shape[0]

    # override values
    for i in range(temp_matrix.cols):
        for j in range(temp_matrix.rows):
            unlabeled_data_np[j][i] = temp_matrix.data_matrix[i][j]

    lambdas = []
    skews = []
    error_codes = []
    for i in range(temp_matrix.cols):
        lambdas.append(temp_matrix.lambdas[i])
        skews.append(temp_matrix.skews[i])
        error_codes.append(temp_matrix.error_codes[i])

    if max(error_codes) > 0:
        exception_handling(error_codes)
        raise Exception('Error in Yeo Johnson transformation. Try automated_yeo_johnson_power_transformation '
                        'for automated parameter search.')

    return Result(
        unlabeled_transformed_data_np=unlabeled_data_np,
        lambdas=lambdas,
        skews=skews,
        error_codes=error_codes,
    )


def automated_yeo_johnson_power_transformation(path_to_c_library, unlabeled_data_np):
    lambda_interval_start=-3
    lambda_interval_end=3
    transformed_unlabeled_data_np, lambdas, skews, error_codes = yeo_johnson_power_transformation(
        path_to_c_library=path_to_c_library,
        unlabeled_data_np=unlabeled_data_np,  # 2D numpy array expected
        interval_start=lambda_interval_start,  # lower bound for lambda
        interval_end=lambda_interval_end,  # upper bound for lambda
        interval_parameter=7,  # precision
        standardize=True,
        time_stamps=False,
        number_of_threads=4,
    )
    counter = 0
    while counter < 100 and max(error_codes) > 0:
        lambda_error = False
        for error_code in error_codes:
            nibble = error_code & 0xF000
            if nibble ^ 0x1000 == 0x0000:
                print("Error occurred during lambda search. Adapting lambda.")
                lambda_error = True
                break

        if lambda_error:
            lambda_interval_start += 1
            lambda_interval_end -= 1
            assert lambda_interval_start < 0 < lambda_interval_end

            transformed_unlabeled_data_np, lambdas, skews, error_codes = yeo_johnson_power_transformation(
                path_to_c_library=path_to_c_library,
                unlabeled_data_np=unlabeled_data_np,  # 2D numpy array expected
                interval_start=lambda_interval_start,  # lower bound for lambda
                interval_end=lambda_interval_end,  # upper bound for lambda
                interval_parameter=14,  # precision
                standardize=True,
                time_stamps=False,
                number_of_threads=1,
            )
        counter += 1
        print(counter)

    return Result(
        unlabeled_transformed_data_np=transformed_unlabeled_data_np,
        lambdas=lambdas,
        skews=skews,
        error_codes=error_codes,
    )


def exception_handling(error_codes, lambda_interval_tuple=None):
    for error_code in error_codes:
        # note: nibble is a half byte

        # first nibble error code segment - super function
        nibble = error_code & 0xF000
        if nibble ^ 0x1000 == 0x0000:
            print("Error occurred during lambda search")
        elif nibble ^ 0x2000 == 0x0000:
            print("Error occurred while transforming data")

        # second nibble error code segment - function
        nibble = error_code & 0x0F00
        if nibble ^ 0x0100 == 0x0000:
            print(" Error occurred while testing skewness")
        elif nibble ^ 0x0200 == 0x0000:
            print(" Error occurred during Yeo Johnson")
        elif nibble ^ 0x0300 == 0x0000:
            print(" Failed to allocate memory")

        # third nibble error code segment - branch
        nibble = error_code & 0x00F0
        if nibble ^ 0x0010 == 0x0000:
            print("     Exception in Yeo Johnson formular 1")
        elif nibble ^ 0x0020 == 0x0000:
            print("     Exception in Yeo Johnson formular 2")
        elif nibble ^ 0x0030 == 0x0000:
            print("     Exception in Yeo Johnson formular 3")
        elif nibble ^ 0x0040 == 0x0000:
            print("     Exception in Yeo Johnson formular 4")
        elif nibble ^ 0x0050 == 0x0000:
            print("     Exception while calculating median")
        elif nibble ^ 0x0060 == 0x0000:
            print("     Exception while calculating standard deviation")
        elif nibble ^ 0x0070 == 0x0000:
            print("     Exception while calculating skew")
        elif nibble ^ 0x0080 == 0x0000:
            print("     Exception in function isCloserToZero (unknown path #BUG)")

        # fourth nibble error code segment - error identifier
        nibble = error_code & 0x000F
        if nibble ^ 0x0001 == 0x0000:
            print("         Vector is null (Null Pointer Exception)")
        elif nibble ^ 0x0002 == 0x0000:
            print("         Not enough rows (calculating skew needs at least 3 rows)")
        elif nibble ^ 0x0003 == 0x0000:
            print(
                "         Value exceeded limit (one value inside vector is too large for calculation)"
            )
        elif nibble ^ 0x0004 == 0x0000:
            print(
                "         Value not inside boundary box (combination of y in vector and lambda results in overflow)"
            )
        elif nibble ^ 0x0005 == 0x0000:
            print("         Boundary Box is not set (unknown path #BUG)")
