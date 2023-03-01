import pandas as pd
from c_accesspoint import yeo_johnson_power_transformation

# adapt data path and parsing
dataFrame = pd.read_csv("../x64/data/generated_test.csv", header=0, low_memory=False)
dataFrame = dataFrame.iloc[:, 1:100]
my_unlabeled_data_np = dataFrame.to_numpy()
path_to_c_library = "../x64/bin/comInterface.so"

data, lambdas, skews, error_codes = yeo_johnson_power_transformation(
    path_to_c_library=path_to_c_library,
    unlabeled_data_np=my_unlabeled_data_np,  # 2D numpy array expected
    interval_start=-200,  # lower bound for lambda
    interval_end=200,  # upper bound for lambda
    interval_parameter=14,  # precision
    standardize=True,
    time_stamps=False,
    number_of_threads=1,
)

print(max(error_codes))
print(hex(error_codes[0]))

