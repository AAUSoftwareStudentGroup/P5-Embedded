import numpy as np


def calculate_accuracy(array):
    dim = len(array[0])
    result = np.zeros(dim, dtype=object)
    for sub_array in array:
        for n in xrange(dim):
            result[n] = result[n] + sub_array[n]
    return result