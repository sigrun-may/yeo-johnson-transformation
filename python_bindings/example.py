import pandas as pd
from c_accesspoint import yeo_johnson_power_transformation

# adapt data path and parsing
dataFrame = pd.read_csv("../x64/data/generated_test.csv", header=0, low_memory=False)
dataFrame = dataFrame.iloc[:, 1:100]
my_unlabeled_data_np = dataFrame.to_numpy()
path_to_c_library = "../x64/bin/comInterface.dll"

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

print("Max of error codes")
print(max(error_codes))
print("Value of first value")
print(hex(error_codes[0]))


for error_code in error_codes:
    # note: nibble is a half byte

    #first nibble error code segment - super function 
    nibble = error_code & 0xF000
    if (nibble ^ 0x1000 == 0x0000):
        print("Error occured during lambda search")
    elif (nibble ^ 0x2000 == 0x0000):
        print("Error occured while transforming data")

    #second nibble error code segment - function
    nibble = error_code & 0x0F00
    if (nibble ^ 0x0100 == 0x0000):
        print(" Error occured while testing skewness")
    elif (nibble ^ 0x0200 == 0x0000):
        print(" Error occured during Yeo Johnson")
    elif (nibble ^ 0x0300 == 0x0000):
        print(" Failed to allocate memory")

    #third nibble error code segment - branch
    nibble = error_code & 0x00F0
    if (nibble ^ 0x0010 == 0x0000):
        print("     Exception in Yeo Johnson formular 1")
    elif (nibble ^ 0x0020 == 0x0000):
        print("     Exception in Yeo Johnson formular 2")
    elif (nibble ^ 0x0030 == 0x0000):
        print("     Exception in Yeo Johnson formular 3")
    elif (nibble ^ 0x0040 == 0x0000):
        print("     Exception in Yeo Johnson formular 4")
    elif (nibble ^ 0x0050 == 0x0000):
        print("     Exception while calculating median")
    elif (nibble ^ 0x0060 == 0x0000):
        print("     Exception while calculating standard deviation")
    elif (nibble ^ 0x0070 == 0x0000):
        print("     Exception while calculating skew")
    elif (nibble ^ 0x0080 == 0x0000):
        print("     Exception in function isCloserToZero (unkown path #BUG)")

    #fourth nibble error code segment - error identifier
    nibble = error_code & 0x000F
    if(nibble ^ 0x0001 == 0x0000):
        print("         Vector is null (Null Pointer Exception)")
    elif(nibble ^ 0x0002 == 0x0000):
        print("         Not enough rows (calculating skew needs at least 3 rows)")
    elif(nibble ^ 0x0003 == 0x0000):
        print("         Value exceeded limit (one value inside vector is too large for calculation)")
    elif(nibble ^ 0x0004 == 0x0000):
        print("         Value not inside boundary box (combination of y in vector and lambda results in overflow)")
    elif(nibble ^ 0x0005 == 0x0000):
        print("         Boundary Box is not set (unkown path #BUG)")