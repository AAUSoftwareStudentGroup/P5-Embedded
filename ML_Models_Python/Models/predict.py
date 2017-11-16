from Data_Classes.classes import Label
from Data_Classes.classes import LabelKind
import numpy as np
# instead of label_dict then use clf.classes_ this will work with scikit check if it works with keras


def predict_tests(clf, tests, label_dict, labelkind):
    preds = []
    # check if keras got this aswell
    if labelkind == LabelKind.Ordinary:
        label_dict = clf.classes_
    for i in xrange(len(tests)):
        test_data = tests[i]
        pred_probabilities = clf.predict_proba(test_data)
        mean_shot_probability = np.mean(pred_probabilities, axis=0)
        print "mean_shot"
        print mean_shot_probability
        preds.append(create_confidence_labels(mean_shot_probability, label_dict))

    return preds


def create_confidence_labels(label_probas, label_dict):
    labels = []
    for i in xrange(len(label_dict)):
        label = Label(label_dict[i], label_probas[i])
        labels.append(label)
    return labels