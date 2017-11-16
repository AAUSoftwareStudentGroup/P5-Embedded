import numpy as np
from Models.kmeans import group_shots, get_center_cords
from Data_Processing.process_helpers import convert_to_np_array
from Feature_Extraction.get_stats_from_shots import make_statistic_features
from sklearn.preprocessing import StandardScaler
from keras.preprocessing.sequence import pad_sequences


def make_features(shots, input_arg, model_name):
    scaler = StandardScaler()
    if (model_name == 'rnn') or (model_name == 'simplernn') or (model_name == 'stackedrnn'):
        result = transform_and_squash(shots,10)
        result = np.array(result)
        # rescale this
        return result
    elif model_name == 'rnns':
        shot_array = make_ten_datapoints_per_shot(shots)
        # now make a rolling window of size 3
        result = rolling2d(shot_array, 3, shot_array.shape[1])
        print result.shape
        return result
    else:
        subset_shots_data = convert_to_np_array(shots)
        f = get_feature_method(input_arg)
        result = f(subset_shots_data)
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

# shape [length_shots, 40]
# one datapoint is x,y,z and rx
def make_ten_datapoints_per_shot(shots):
    result_arrays = np.zeros((len(shots), 40))
    for c, shot in enumerate(shots):
        intermediate_array = np.zeros(40)
        current_index = 0
        for i in xrange(10):
            array = np.array([shot['x'][i], shot['y'][i], shot['z'][i], shot['rx'][i]])
            # edit this shit make sure everything is added in the correct order to intermediate array
            intermediate_array[current_index] = shot['x'][i]
            intermediate_array[current_index + 1] = shot['y'][i]
            intermediate_array[current_index + 2] = shot['z'][i]
            intermediate_array[current_index + 3] = shot['rx'][i]
            current_index = current_index + 4
        result_arrays[c] = intermediate_array
    return result_arrays

def rolling2d(data, window_size, n_features):
    data_windows = np.zeros((data.shape[0] - window_size + 1, window_size, n_features))
    for i in xrange(data.shape[0] - window_size + 1):
        windows = np.zeros((window_size, n_features))
        j = 0
        while j < window_size:
            windows[j] = data[i + j]
            j = j + 1
        data_windows[i] = windows

    return data_windows


# shape [length_shots, 10, 4]
def transform_and_squash(shots, size):
    segments = []
    scaler = StandardScaler()
    for shot in shots:
        segment = np.zeros((size, 4))
        for i in xrange(size):
            segment[i] = np.array([shot['x'][i], shot['y'][i], shot['z'][i], shot['rx'][i]])
        segment = scaler.fit(segment).transform(segment)
        segments.append(segment)

    return segments


def padding(sequences):
    pad_sequences(sequences, padding='post')