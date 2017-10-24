from numba import njit
import numpy as np


def make_acc_angle(df, acc_sensitivity):
    df['x_ang'] = df['x'] / acc_sensitivity
    df['y_ang'] = df['y'] / acc_sensitivity
    df['z_ang'] = df['z'] / acc_sensitivity
    return df


def make_gyro_rate(df, gyro_sensitivity):
    df['rx_rate'] = df['rx'] / gyro_sensitivity
    df['ry_rate'] = df['ry'] / gyro_sensitivity
    df['rz_rate'] = df['rz'] / gyro_sensitivity
    return df


def get_sample_frequency(sample_times):
    for i in xrange(1, len(sample_times)):
        if sample_times[i] >= 1000:
            return i


#def shuffle(np_array):
#    np.random.shuffle(np_array)


def smoothing(df, alpha):
    df[['x', 'y', 'z', 'rx', 'ry', 'rz']] = df[['x', 'y', 'z', 'rx', 'ry', 'rz']].ewm(com = alpha).mean()
    return df


@njit
def complementary_filter(col_rx_rate, col_x_ang,sf,length):
    dt = float(1.0 / sf)
    alpha = 0.2
    roll = np.zeros(length)
    roll[0] = (1 - alpha) * (col_rx_rate[0] * dt) + alpha * col_x_ang[0]
    for i in xrange(1, length):
        roll[i] = (1 - alpha) * (roll[i - 1]) + (col_rx_rate[i] * dt) + alpha * col_x_ang[i]
    return (roll)


def calc_toal_acc(df, sens):
    df['total_acc'] = np.sqrt((df['x']**2) + (df['y']**2) + (df['z']**2))
    # dividing by the acc_sensitivity
    df['total_acc'] = df['total_acc'] / 2048
    return df


def calc_noise(df):
    df['no_noise'] = (df['total_acc'] > 1.2) | (df['rx'] > 2000)
    return df


def get_shots_index(indexes,max_dist, min_lenght):
    shots_indexes = []
    start_index = indexes[0]
    length = len(indexes)
    for i in xrange(1, length):
        if i == length - 1:
            break
        elif (indexes[i + 1] - indexes[i]) > max_dist:
            if (indexes[i] - start_index) < min_lenght:
                start_index = indexes[i + 1]
            else:
                shots_indexes.append((start_index, indexes[i] + 1))
                start_index = indexes[i + 1]
    return shots_indexes


# only take x,y,z, rx
def extract_shots(df, indexes):
    shots = np.empty(len(indexes), dtype=object)
    for count, pair in enumerate(indexes):
        # we dont wan't the entire df only those we gonna use for features to ANN
        shots[count] = df.iloc[pair[0]:pair[1]][['x', 'y', 'z', 'rx']].values
    return shots