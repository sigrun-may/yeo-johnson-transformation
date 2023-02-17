from collections import namedtuple
from ctypes import CDLL

from scipy.stats import yeojohnson, skew
from typing import Tuple

import numpy as np
import pandas as pd
from sklearn.preprocessing import PowerTransformer, StandardScaler
from sklearn.utils import shuffle

from python_bindings import c_accesspoint

TrainTestSplit = namedtuple("TrainTestSplit", ["train_np", "test_np"])


# reference implementation
def _transform_sklearn() -> Tuple[np.ndarray, np.ndarray]:
    # transform and standardize test and train data
    power_transformer = PowerTransformer(
        copy=True, method="yeo-johnson", standardize=True
    )
    transformed_train_np = power_transformer.fit_transform(train_np)
    transformed_test_np = power_transformer.transform(test_np)
    return TrainTestSplit(train_np=transformed_train_np, test_np=transformed_test_np)


def _transform_yeo_johnson_c() -> Tuple[np.ndarray, np.ndarray]:
    # transform and standardize test and train data
    transformed_train_np, lambdas, skews = c_accesspoint.yeo_johnson(
        yeo_johnson_c=CDLL('../x64/bin/comInterface.so').ciParallelOperation,
        # c_data_type=ctypes.c_double,
        matrix=train_np,
        interval_start=-25,
        interval_end=25,
        interval_parameter=14,
        standardize=1,
        time_stamps=0,
        thread_count=1,
    )

    # transform test data
    transformed_test_np_c = np.zeros_like(test_np)
    transformed_test_np_stats = np.zeros_like(test_np)
    for i in range(test_np.shape[1]):
        transformed_test_np_c[:, i] = yeojohnson(test_np[:, i], lambdas[i])

        # calculate lambda for column in train data
        _, lamp = yeojohnson(train_np[:, i])
        print(i, 'lambdas stats:', lamp, 'c:', lambdas[i])

        # apply lambda to equal test data
        transformed_test_np_stats[:, i] = yeojohnson(test_np[:, i], lamp)

    # standardize test data  # TODO standardize parameter from c code
    scaler = StandardScaler()
    scaler.fit(train_np)
    transformed_test_np_c = scaler.transform(transformed_test_np_c, copy=False)
    transformed_test_np_stats = scaler.transform(transformed_test_np_stats, copy=False)

    print('skew c orginal without transformation')
    print(np.asarray(skews))
    print('skew c')
    print(skew(transformed_test_np_c, axis=0))
    print('skew stats')
    print(skew(transformed_test_np_stats, axis=0))

    train, test = _transform_sklearn()
    np.testing.assert_almost_equal(skew(transformed_test_np_stats, axis=0), skew(test, axis=0), decimal=7)

    print('-------------------------------------------')
    print('skew sklean train')
    print(skew(train, axis=0))
    print('skew train c')
    print(skew(transformed_train_np, axis=0))

    return TrainTestSplit(train_np=transformed_train_np, test_np=transformed_test_np_c)


# generate test data
data_df = pd.read_csv('../x64/data/artificial_20_20.csv')
print("data shape:", data_df.shape)
shuffled_data_df = shuffle(data_df)
test_np = shuffled_data_df.iloc[:5, 1:].values
train_np = shuffled_data_df.iloc[5:, 1:].values

# run checks
train_c, test_c = _transform_yeo_johnson_c()
