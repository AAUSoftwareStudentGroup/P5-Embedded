from Models.clf import create_classifier
from Data_Classes.classes import Label
from Data_Classes.classes import LabelKind
import numpy as np


def create_classifier_helper(model_name, init_param, input_shape=None, output_shape=None):
    clf = create_classifier(model_name, init_param, input_shape, output_shape)
    return clf


def teach_model(clf, shots, labels, learn_param):
    clf.learn(shots, labels, **learn_param)


def ANN_predict(shots, labels, init_param, learn_param):
    clf = create_classifier_helper("ANN", learn_param, shots.shape[1:], labels.shape[1])
    teach_model(clf, shots, labels, learn_param)
    return clf


def RNN_predict(shots, labels, init_param, learn_param):
    #shots = np.reshape(shots, (shots.shape[0], 1, shots.shape[1]))
    clf = create_classifier_helper("RNN", init_param, shots.shape[1:], labels.shape[1])
    teach_model(clf, shots, labels, learn_param)
    return clf


def SVM_predict(shots, labels, init_param, learn_param):
    clf = create_classifier_helper("SVM", init_param)
    teach_model(clf, shots, labels, learn_param)
    return clf


def LR_predict(shots, labels, init_param, learn_param):
    clf = create_classifier_helper("LR", init_param, shots.shape[1:], labels.shape[1])
    teach_model(clf, shots, labels, learn_param)
    return clf


class Classifications:
    def __init__(self, preds):
        self.preds = preds


def create_confidence_labels(label_probas, label_dict):
    labels = []
    for i in xrange(len(label_dict)):
        label = Label(label_dict[i], label_probas[i])
        labels.append(label)
    return labels


# instead of label_dict then use clf.classes_ this will work with scikit check if it works with keras
def predict_tests(clf, tests, label_dict, labelkind):
    preds = []
    # check if keras got this aswell
    if labelkind == LabelKind.Ordinary:
        label_dict = clf.classes_
    for i in xrange(len(tests)):
        test_data = tests[i]
        pred_probabilities = clf.predict_proba(test_data)
        mean_shot_probability = np.mean(pred_probabilities, axis = 0)
        print "mean_shot"
        print mean_shot_probability
        preds.append(create_confidence_labels(mean_shot_probability, label_dict))

    return preds