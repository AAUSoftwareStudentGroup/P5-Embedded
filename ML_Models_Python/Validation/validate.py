import numpy as np


def cross_validate(trained_models, test_data, expected_labels):
    model_dict = {}
    for model in trained_models:
        for test in test_data:
            accuracy = model.predict_proba()
            tp = calculate_tp(accuracy, expected_labels)
        model_dict[trained_models.name] = tp


def calculate_tp(accuracy, expected_labels):
    # tp = true positivies
    tp = np.zeros(len(expected_labels))
    for j, acc in enumerate(accuracy):
        max_val = (-1, -1)
        for i in xrange(len(acc)):
            if acc[i] > max_val[1]:
                max_val[0] = i
                max_val[1] = acc[i]
        if isinstance(expected_labels, np.ndarray):
                tp[j] = expected_labels(max_val[0]) is 1
    return np.sum(tp)