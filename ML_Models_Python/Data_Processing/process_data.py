import numpy as np
from Models.kmeans import group_shots, get_center_cords
from Data_Processing.process_helpers import convert_to_np_array
from Feature_Extraction.get_stats_from_shots import make_statistic_features
from sklearn.preprocessing import StandardScaler


def make_features(shots, input_arg, model_name):
    if model_name == 'rnn':
        result = squash_shots(shots)
        result =  np.array(result)
        # rescale this
        return result
    if not model_name == 'rnn':
        subset_shots_data = convert_to_np_array(shots)
        f = get_feature_method(input_arg)
        result = f(subset_shots_data)
        scaler = StandardScaler()
        return scaler.fit(result).transform(result)


def k_groups(shots):
    movements_grouped = group_shots(shots, 5)
    return np.array(get_center_cords(movements_grouped, 5))


def shot_stats(shots):
    return make_statistic_features(shots)


def raw_data(shots):
    return shots


def get_feature_method(argument):
    mappings = {
        "k_groups": k_groups,
        "shot_stats": shot_stats,
        "raw": raw_data
    }
    if argument not in mappings:
        raise ValueError("Selected Method for Feature Extraction does not exist")
    # Get the function from switcher dictionary
    func = mappings.get(argument, lambda: "nothing")
    # Execute the function
    return func


# shape [length_shots, 10, 4]
def squash_shots(shots):
    segments = []
    for shot in shots:
        segment = np.zeros((10, 4))
        for i in xrange(10):
            segment[i] = np.array([shot['x'][i], shot['y'][i], shot['z'][i], shot['rx'][i]])
        scaler = StandardScaler()
        segment = scaler.fit(segment).transform(segment)
        segments.append(segment)
    return segments