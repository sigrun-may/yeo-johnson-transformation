from collections import namedtuple
from ctypes import *
import platform
import numpy as np


class MATRIX(Structure):
    _fields_ = [
        ("rows", c_int),
        ("cols", c_int),
        ("data", POINTER(POINTER(c_double))),
        ("lambdas", POINTER(c_double)),
        ("skew", POINTER(c_double)),
        ("errnum", POINTER(c_int)), # <-- new
    ]


def _construct_c_matrix(matrix, c_data_type):
    #  getting dimensions
    column_dimension = np.size(matrix, 1)
    row_dimension = np.size(matrix, 0)
    #  initialising result vectors
    ptr_lambdas = (c_data_type * column_dimension)()
    ptr_skews = (c_data_type * column_dimension)()
    ptr_errnum = (c_int * column_dimension)() # <-- new
    #  initialising pointer array
    data_matrix = (POINTER(c_data_type) * column_dimension)()
    #  filling pointer array with values from matrix
    for i in range(column_dimension):
        data_row = (c_data_type * row_dimension)()
        for j in range(row_dimension):
            value = matrix[j][i]
            data_row[j] = c_data_type(value)
        data_matrix[i] = data_row
    #  transforming matrix into c format
    return MATRIX(row_dimension, column_dimension, data_matrix, ptr_lambdas, ptr_skews, ptr_errnum)

def yeo_johnson(
    matrix,
    interval_start,
    interval_end,
    interval_parameter,
    standardize,
    time_stamps,
    thread_count,
):
    # accessing c functionality
    os = platform.system()
    if os == 'Linux':
        yeo_johnson_c = CDLL("../x64/bin/comInterface.so").ciParallelOperation
    elif os == 'Windows':
        yeo_johnson_c = CDLL("../x64/bin/comInterface.dll").ciParallelOperation
    else:
        print("Please generate your own library and add the link here.")
        raise EnvironmentError("Only Linux and Windows is supported.")
    # defining parameters
    yeo_johnson_c.argtypes = [
        c_double,
        c_double,
        c_int,
        POINTER(MATRIX),
        c_int,
        c_int,
        c_int,
    ]
    #defining return type
    yeo_johnson_c.restype = c_int

    # Returning matrix with adjusted values
    #  constructing c struct
    temp_matrix = _construct_c_matrix(matrix, c_double)

    # check if calculation is serial
    assert thread_count >= 1
    
    #calculating
    yeo_johnson_c(
        interval_start,
        interval_end,
        interval_parameter,
        pointer(temp_matrix),
        standardize,
        time_stamps,
        thread_count,
    )
    
    # override values
    for i in range(temp_matrix.cols):
        for j in range(temp_matrix.rows):
            matrix[j][i] = temp_matrix.data[i][j]

    Result = namedtuple("Result", "matrix lambdas skews errnum")

    lambdas = []
    skews = []
    errnum = []
    for i in range(temp_matrix.cols):
        lambdas.append(temp_matrix.lambdas[i])
        skews.append(temp_matrix.skew[i])
        errnum.append(temp_matrix.errnum[i])

    result = Result(matrix=matrix, lambdas=lambdas, skews=skews, errnum=errnum)

    return result
