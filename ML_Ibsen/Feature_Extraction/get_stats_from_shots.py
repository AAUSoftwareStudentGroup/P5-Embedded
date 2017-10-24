from scipy import stats
import numpy as np


def make_statistic_features(shots):
    shots_features = np.zeros((len(shots), 16))
    for c, shot in enumerate(shots):
        x_col = shot[:, 0]
        y_col = shot[:, 1]
        z_col = shot[:, 2]
        roll_col = shot[:, 3]

        x_stats = stats_extracter(x_col)
        y_stats = stats_extracter(y_col)
        z_stats = stats_extracter(z_col)
        roll_stats = stats_extracter(roll_col)

        combined_stats = np.concatenate([x_stats, y_stats, z_stats, roll_stats])

        shots_features[c] = combined_stats
    return shots_features


def stats_extracter(col):
    result = np.zeros(4)
    statistics = stats.describe(col)

    # min
    result[0] = statistics.minmax[0]
    # max
    result[1] = statistics.minmax[1]
    result[2] = statistics.mean
    result[3] = len(col)
    return result
