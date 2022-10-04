from ctypes import *
import ctypes
from tokenize import Double
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

class MATRIX(Structure):
    _fields_ = [("rows", c_int),
                ("cols", c_int),
                ("data", POINTER(POINTER(c_float))),
                ("lambdas", POINTER(c_float)),
                ("skew", POINTER(c_float))]

def yeo_johnson_construction(matrix):
    #getting dimensions
    column_dimension = np.size(matrix,1)
    row_dimension = np.size(matrix,0)
    #initialising result vectors
    ptr1 = (ctypes.c_float*column_dimension)()
    ptr2 = (ctypes.c_float*column_dimension)()
    #initialising pointer array
    data_matrix = (POINTER(ctypes.c_float)*column_dimension)()
    #filling pointer array with values from matrix
    for i in range(column_dimension):
        data_row = (ctypes.c_float * row_dimension)()
        for j in range(row_dimension):
            value = matrix[j][i]
            data_row[j] = c_float(value)
        data_matrix[i] = data_row
    #transforming matrix into c format
    return MATRIX(row_dimension, column_dimension, data_matrix, ptr1, ptr2)

def yeo_johnson_output(yeo_johnson_c, matrix, interval_start, interval_end, interval_parameter, standardize, time_stamps):
    #constructing c struct
    temp_matrix = yeo_johnson_construction(matrix)
    #calculating
    yeo_johnson_c(interval_start, interval_end, interval_parameter, pointer(temp_matrix), standardize, time_stamps)
    #override values
    for i in range(temp_matrix.cols):
        for j in range(temp_matrix.rows):
            matrix[j][i] = temp_matrix.data[i][j]
    #returning lambdas and skew
    result = np.zeros(shape=(temp_matrix.cols, 2))
    for i in range(temp_matrix.cols):
        lambdas = temp_matrix.lambdas[i]
        result[i][0] = lambdas
        skew = temp_matrix.skew[i]
        result[i][1] = skew
    return result

def yeo_johnson(matrix, interval_start, interval_end, interval_parameter):
    #accessing c functionality
    shared_library_path = "./x86/bin/comInterface.dll"
    yeo_johnson_c = CDLL(shared_library_path).ciLambdaOperation
    yeo_johnson_c.argtypes = [c_float, c_float, c_float, POINTER(MATRIX), c_int, c_int]
    yeo_johnson_c.restype = c_int
    # Returning matrix with adjusted values
    return yeo_johnson_output(yeo_johnson_c, matrix, interval_start, interval_end, interval_parameter)

def yeo_johnson_smart(matrix, interval_start, interval_end, interval_parameter, standardize, time_stamps):
    #accessing c functionality
    shared_library_path = "./x86/bin/comInterface.dll"
    yeo_johnson_c = CDLL(shared_library_path).ciSmartOperation
    yeo_johnson_c.argtypes = [c_float, c_float, c_int, POINTER(MATRIX), c_int, c_int]
    yeo_johnson_c.restype = c_int
    # Returning matrix with adjusted values
    return yeo_johnson_output(yeo_johnson_c, matrix, interval_start, interval_end, interval_parameter, standardize, time_stamps)


# EXAMPLE
# dataFrame = pd.read_csv("./x86/data/generated_test.csv", header=0, low_memory=False)
# dataFrame = dataFrame.iloc[: , 1:]
# dataFrame = dataFrame.astype(float)
# my_data = dataFrame.to_numpy()
# output = yeo_johnson_smart(my_data, -2, 2, 14, 1, 1) #matrix, interval_start, interval_end, precision, standardize, time_stamps
# print("output=\n")
# print(output)
# print("test_matrix 1\n")
# print(my_data)