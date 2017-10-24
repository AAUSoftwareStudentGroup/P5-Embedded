from Data_Processing.process_data import process_data_v2
import numpy as np


def create_tests_v2(tests, invert=False):

    result = []
    for test in tests:
        processed_data = process_data_v2(test.df, "shot_stats")
        if invert:
            result.append(np.reshape(processed_data, (processed_data.shape[0], 1, processed_data.shape[1])))
        else:
            result.append(processed_data)
    return np.array(result)