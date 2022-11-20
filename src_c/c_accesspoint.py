import ctypes
from ctypes import *

import numpy as np
import pandas as pd


class MATRIX(Structure):
    _fields_ = [
        ("rows", c_int),
        ("cols", c_int),
        ("data", POINTER(POINTER(c_double))),
        ("lambdas", POINTER(c_double)),
        ("skew", POINTER(c_double)),
    ]


class MATRIXF(Structure):
    _fields_ = [
        ("rows", c_int),
        ("cols", c_int),
        ("data", POINTER(POINTER(c_float))),
        ("lambdas", POINTER(c_float)),
        ("skew", POINTER(c_float)),
    ]


def _construct_c_matrix(matrix, c_data_type):
    #  getting dimensions
    column_dimension = np.size(matrix, 1)
    row_dimension = np.size(matrix, 0)
    #  initialising result vectors
    ptr_lambdas = (c_data_type * column_dimension)()
    ptr_skews = (c_data_type * column_dimension)()
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
    if c_data_type is ctypes.c_double:
        return MATRIX(row_dimension, column_dimension, data_matrix, ptr_lambdas, ptr_skews)
    else:
        return MATRIXF(row_dimension, column_dimension, data_matrix, ptr_lambdas, ptr_skews)


def _yeo_johnson_output(
        yeo_johnson_c,
        c_data_type,
        matrix,
        interval_start,
        interval_end,
        interval_parameter,
        standardize,
        time_stamps,
        thread_count,
):
    #  constructing c struct
    temp_matrix = _construct_c_matrix(matrix, c_data_type)
    # calculating
    if thread_count == 0:
        yeo_johnson_c(
            interval_start,
            interval_end,
            interval_parameter,
            pointer(temp_matrix),
            standardize,
            time_stamps,
        )
    else:
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
    # returning lambdas and skew
    result = np.zeros(shape=(temp_matrix.cols, 2))
    for i in range(temp_matrix.cols):
        lambdas = temp_matrix.lambdas[i]
        result[i][0] = lambdas
        skew = temp_matrix.skew[i]
        result[i][1] = skew
    return result


def yeo_johnson(
        yeo_johnson_c,
        c_data_type,
        matrix,
        interval_start,
        interval_end,
        interval_parameter,
        standardize,
        time_stamps,
        thread_count,
):
    # accessing c functionality
    if c_data_type is ctypes.c_double:
        yeo_johnson_c.argtypes = [
            c_data_type,
            c_data_type,
            c_int,
            POINTER(MATRIX),
            c_int,
            c_int,
            c_int,
        ]
    else:
        assert c_data_type is ctypes.c_float
        yeo_johnson_c.argtypes = [
            c_data_type,
            c_data_type,
            c_int,
            POINTER(MATRIXF),
            c_int,
            c_int,
            c_int,
        ]
    yeo_johnson_c.restype = c_int
    # Returning matrix with adjusted values
    return _yeo_johnson_output(
        yeo_johnson_c,
        c_data_type,
        matrix,
        interval_start,
        interval_end,
        interval_parameter,
        standardize,
        time_stamps,
        thread_count,
    )


# EXAMPLE
dataFrame = pd.read_csv("../x64/data/generated_test.csv", header=0, low_memory=False)
dataFrame = dataFrame.iloc[:, 1:]
my_data = dataFrame.to_numpy()
print(my_data.shape)
output = yeo_johnson(
    yeo_johnson_c=CDLL("../x64/bin/comInterface.dll").ciParallelOperation,
    c_data_type=ctypes.c_double,
    matrix=my_data,
    interval_start=-2,
    interval_end=2,
    interval_parameter=14,
    standardize=1,
    time_stamps=1,
    thread_count=4,
)  # matrix, interval_start, interval_end, precision, standardize, time_stamps
print("output=\n")
print(output)
print("test_matrix 1\n")
print(my_data)

# # transform and standardize data
# power_transformer = PowerTransformer(
#     copy=True, method="yeo-johnson", standardize=True
# )
# start = time()
# train = power_transformer.fit_transform(my_data)
# print("duration:", timedelta(seconds=time() - start))

# c_function = CDLL("./x64/bin/comInterface.dll")
# res = c_function.ciParallelOperation
