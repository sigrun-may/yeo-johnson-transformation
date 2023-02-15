from ctypes import CDLL
from datetime import datetime

import pandas as pd
from sklearn.preprocessing import PowerTransformer

dataFrame = pd.read_csv("../x64/data/generated_test.csv", header=0, low_memory=False)
dataFrame = dataFrame.iloc[:, 1:10000]
my_data = dataFrame.to_numpy()

# transform and standardize data
power_transformer = PowerTransformer(
    copy=True, method="yeo-johnson", standardize=True
)
start = datetime.now()
train = power_transformer.fit_transform(my_data)
print("duration sklearn:", datetime.now() - start)
print(train.shape)

# Linux
c_function = CDLL("../x64/bin/comInterface.so")

# # Windows
# c_function = CDLL("./x64/bin/comInterface.dll")

start = datetime.now()
res = c_function.ciParallelOperation
print("duration c:", datetime.now() - start)
