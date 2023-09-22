from collections import namedtuple
from ctypes import CDLL, POINTER, c_double, c_int, pointer, Structure

import numpy as np


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
        row_dimension, column_dimension, data_matrix, ptr_lambdas, ptr_skews, ptr_error_codes
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
    #yeo_johnson_c = CDLL(path_to_c_library).ciParallelOperation
    yeo_johnson_c = CDLL(path_to_c_library).ciParallelOperationBowley

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
    exit_code = yeo_johnson_c(
        c_double(interval_start),
        c_double(interval_end),
        c_int(interval_parameter),
        pointer(temp_matrix),
        c_int(standardize),
        c_int(time_stamps),
        c_int(number_of_threads),
    )

    print(exit_code)
    assert temp_matrix.cols == unlabeled_data_np.shape[1]
    assert temp_matrix.rows == unlabeled_data_np.shape[0]

    # override values
    for i in range(temp_matrix.cols):
        for j in range(temp_matrix.rows):
            unlabeled_data_np[j][i] = temp_matrix.data_matrix[i][j]

    Result = namedtuple(
        "Result", "unlabeled_transformed_data_np lambdas skews error_codes"
    )

    lambdas = []
    skews = []
    error_codes = []
    for i in range(temp_matrix.cols):
        lambdas.append(temp_matrix.lambdas[i])
        skews.append(temp_matrix.skews[i])
        error_codes.append(temp_matrix.error_codes[i])

    return Result(
        unlabeled_transformed_data_np=unlabeled_data_np,
        lambdas=lambdas,
        skews=skews,
        error_codes=error_codes,
    )
