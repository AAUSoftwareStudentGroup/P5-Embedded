import numpy as np
from process_helpers import smoothing, make_acc_angle, make_gyro_rate, calc_noise \
    , get_sample_frequency, complementary_filter, calc_toal_acc, extract_shots, get_shots_index
from Models.kmeans import group_shots, get_center_cords
from Feature_Extraction.get_stats_from_shots import make_statistic_features


def process_data_v2(data, input_arg):
    alpha = 0.2
    acc_sensitivity = 2048 / 90
    gyro_sensitivity = 16.4
    data = smoothing(data, alpha)
    data = make_acc_angle(data, acc_sensitivity)
    data = make_gyro_rate(data, gyro_sensitivity)
    sf = get_sample_frequency(data['time'])
    (roll) = complementary_filter(data['rx_rate'].values, data['x_ang'].values, sf, data.shape[0])
    data['roll'] = roll
    data = calc_toal_acc(data, acc_sensitivity)
    data = calc_noise(data)
    df_no_noise = data[data['no_noise']== True]
    indexes = get_shots_index(df_no_noise.index, 20, 30)
    shots = extract_shots(data, indexes)
    # use dict mapping to use different feature extraction methods
    argument = input_arg
    f = get_feature_method(argument)
    result = f(shots)
    return (result)


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
