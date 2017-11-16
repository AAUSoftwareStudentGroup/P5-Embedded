import numpy as np


# remove unneccesary data and convvert to np array which is faster to append
def convert_to_np_array(shots_array):
    shots = np.empty(len(shots_array), dtype=object)
    for i in xrange(len(shots)):
        shots[i] = shots_array[i][['x', 'y', 'z', 'rx']].values

    return shots