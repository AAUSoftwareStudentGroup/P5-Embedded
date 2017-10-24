from Data_Processing.process_data import process_data_v2
from Data_Processing.pred_set import Pred_set
from Models.train_helpers import equal_size_train, merge_trains
import numpy as np


def get_shots_from_trains_dfs(data, labels, feature_method="shot_stats"):
    tests = list()
    is_label_iterable = len(np.array(labels).shape) > 1
    label_dict = {}
    print "blabla"
    print len(labels)
    if len(data) is not len(labels):
        raise ValueError("Add a label for each path")
    for i in xrange(len(data)):
        df = data[i].df
        label_list = labels[i]
        if is_label_iterable:
            label = ''.join(str(nr) for nr in label_list)
        else:
            label = str(labels[i])
        train = process_data_v2(df, feature_method)
        if label not in label_dict:
            label_dict[label] = Pred_set(train, label_list)
        else:
            label_dict[label].data = np.concatenate([label_dict[label].data, train])
    trains = equal_size_train(label_dict)
    (merged_trains, merged_labels) = merge_trains(trains)
    return (merged_trains, merged_labels, tests)