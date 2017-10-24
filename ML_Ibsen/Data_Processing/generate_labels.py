import numpy as np
from Data_Classes.classes import LabelKind


def generate_labels(labels, labelKind):
    # if its a ordinary lable kind for e.g. svm give each label a unique integer number
    if labelKind == LabelKind.Ordinary:
        return labels, None
    #    labels_dict = {}
    #    used_labels = {}
    #    new_labels = []
    #    count = 0
    #    for i in xrange(len(labels)):
    #        if labels[i] not in used_labels:
    #            used_labels[labels[i]] = count
    #            labels_dict[count] = labels[i]
    #            count = count + 1
    #        new_labels.append(used_labels[labels[i]])
    #    return new_labels, labels_dict
    # represent as an array of arrays of labels e.g. [[1, 0], [0,1]]
    if labelKind == LabelKind.Arbitrary:
        labels = np.array(labels)
        # find unique labels
        unique_labels = np.unique(labels)
        len_unique_labels = len(unique_labels)
        # lets make some new labels:
        new_unique_labels = np.zeros((len_unique_labels, len_unique_labels), dtype=np.int)
        labels_dict_name = {}
        labels_dict = {}

        # fill the initilised empty array out
        i = 0
        for j in xrange(len_unique_labels):
            new_unique_labels[j][i] = 1
            i = i + 1
            labels_dict[j] = unique_labels[j]
            labels_dict_name[unique_labels[j]] = new_unique_labels[j]
        new_labels = []
        for label in labels:
            new_labels.append(labels_dict_name[label].tolist())
        return new_labels, labels_dict
