from Models.clf import create_classifier
from Data_Classes.classes import Label
from Data_Classes.classes import LabelKind
import numpy as np


def ANN_predict(shots, labels, parameters):
    clf = create_classifier("ANN", shots.shape[1:], labels.shape[1], parameters)
    clf.learn(shots, labels)
    return clf


def RNN_predict(shots, labels, parameters):
    shots = np.reshape(shots, (shots.shape[0], 1, shots.shape[1]))
    clf = create_classifier("RNN", shots.shape[1:], labels.shape[1], parameters)
    clf.learn(shots, labels)
    return clf


def make_Classification(label_probas, label_dict):
    labels = []
    for i in xrange(len(label_dict)):
        label = Label(label_dict[i], label_probas[i])
        labels.append(label)
    return labels


def SVM_predict(shots, labels, parameters):
    clf = create_classifier("SVM", shots, labels, parameters)
    clf.learn(shots, labels)
    return clf


class Classifications:
    def __init__(self, preds):
        self.preds = preds


# instead of label_dict then use clf.classes_ this will work with scikit check if it works with keras
def predict_tests(clf, tests, model_name, label_dict, labelkind):
    preds = []
    # check if keras got this aswell
    if labelkind == LabelKind.Ordinary:
        label_dict = clf.classes_
    for i in xrange(len(tests)):
        test_data = tests[i]
        if model_name.lower() == 'rnn':
            test_data = np.reshape(test_data, (test_data.shape[0], 1, test_data.shape[1]))
        pred_probabilities = clf.predict_proba(test_data)
        pred_probabilities = np.sum(pred_probabilities, axis=0)
        mean_shot_probability = (pred_probabilities / len(tests[i]))
        preds.append(make_Classification(mean_shot_probability, label_dict))

    return preds