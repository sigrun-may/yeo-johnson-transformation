# EXAMPLE
import ctypes
from ctypes import CDLL

import pandas as pd

from python_bindings.c_accesspoint import yeo_johnson

dataFrame = pd.read_csv("../x64/data/generated_test.csv", header=0, low_memory=False)
dataFrame = dataFrame.iloc[:, 1:]
my_data = dataFrame.to_numpy()
print(my_data.shape)
output = yeo_johnson(
    yeo_johnson_c=CDLL("../x64/bin/comInterface.so").ciParallelOperation,
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


