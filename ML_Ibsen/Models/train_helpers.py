import numpy as np
#from Data_Processing.process_helpers import shuffle


def equal_size_train(train_dict):
    # min val:
    min_val = min([len(value.data) for value in train_dict.values()])
    for key, value in train_dict.items():
        #shuffle(value.data)
        train_dict[key].data = value.data[:min_val]
    return list(train_dict.values())


def extract_test(np_array, n):
    if n > len(np_array):
        raise ValueError('n should not exeed the nr of shots ')
    else:
        # return train,test
        return (np_array[:-n], np_array[-n:])


def merge_trains(array):
    train_list = array[0].data
    label_list = [array[0].label] * len(array[0].data)
    if len(array) > 1:
        for element in xrange(1, len(array)):
            labels_array = [array[element].label] * len(array[element].data)
            label_list = np.concatenate([label_list, labels_array])
            train_list = np.concatenate([train_list, array[element].data])

    return (train_list, label_list)