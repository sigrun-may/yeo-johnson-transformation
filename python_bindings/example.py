import pandas as pd
from python_bindings.c_accesspoint import yeo_johnson

# adapt data path and parsing
dataFrame = pd.read_csv("../x64/data/generated_test.csv", header=0, low_memory=False)
dataFrame = dataFrame.iloc[:, 1:]
my_data = dataFrame.to_numpy()

output = yeo_johnson(
    matrix=my_data,  # 2D numpy array expected
    interval_start=-2,  # lower bound for lambda
    interval_end=2,  # upper bound for lambda
    interval_parameter=14,  # precision
    standardize=1,  # 1 = true, 0 = false
    time_stamps=1,  # 1 = true, 0 = false
    thread_count=4,
)
