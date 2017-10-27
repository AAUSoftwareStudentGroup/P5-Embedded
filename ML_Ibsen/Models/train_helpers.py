import numpy as np


def equal_size_train(train_dict):
    min_val = min([len(value.data) for value in train_dict.values()])
    for key, value in train_dict.items():
        train_set = train_dict[key].data
        if len(train_set) > min_val:
            np.random.shuffle(value.data)
        train_dict[key].data = value.data[:min_val]
    return list(train_dict.values())


def merge_trains(array):
    if len(array) == 0:
        raise ValueError("cannot train on empty data")
    train_list = array[0].data
    label_list = [array[0].label] * len(array[0].data)
    if len(array) > 1:
        for element in xrange(1, len(array)):
            labels_array = [array[element].label] * len(array[element].data)
            label_list = np.concatenate([label_list, labels_array])
            train_list = np.concatenate([train_list, array[element].data])

    return (train_list, label_list)