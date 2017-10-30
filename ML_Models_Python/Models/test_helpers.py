from Data_Processing.process_data import make_features
import numpy as np


def create_tests(tests, model_name, invert=False):
    result = []
    for test in tests:
        processed_data = make_features(test.shots, "shot_stats", model_name)
        if invert:
            result.append(np.reshape(processed_data, (processed_data.shape[0], 1, processed_data.shape[1])))
        else:
            result.append(processed_data)
    return np.array(result)