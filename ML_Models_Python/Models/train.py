from Data_Processing.process_data import make_features
from Data_Processing.pred_set import Pred_set
from Models.train_helpers import equal_size_train, merge_trains
import numpy as np


def get_features_from_shots(data, labels, model_name, feature_method="shot_stats"):
    print ("len of train_set: {}").format(len(data))
    is_label_iterable = len(np.array(labels).shape) > 1
    label_dict = {}
    if len(data) is not len(labels):
        raise ValueError("Add a label for each train set")
    for i in range(len(data)):
        shots = data[i].shots
        label_list = labels[i]
        if is_label_iterable:
            label = ''.join(str(nr) for nr in label_list)
        else:
            label = str(labels[i])
        train = make_features(shots, feature_method, model_name)
        if label not in label_dict:
            label_dict[label] = Pred_set(train, label_list)
        else:
            label_dict[label].data = np.concatenate([label_dict[label].data, train])
    trains = equal_size_train(label_dict)
    (merged_trains, merged_labels) = merge_trains(trains)
    return (np.array(merged_trains), np.array(merged_labels))