import numpy as np
from Data_Classes.classes import LabelKind
from keras.utils import np_utils


def generate_labels(labels, labelKind):
    if labelKind == LabelKind.Ordinary:
        return labels, None
    if labelKind == LabelKind.Arbitrary:
        uniques, indexes = np.unique(labels, return_inverse=True)
        new_labels = np_utils.to_categorical(indexes, len(uniques))
        return new_labels, uniques