def predict(clf, test_pred_sets):
    if not len(test_pred_sets) > 0 and test_pred_sets[0].data is not False:
        for test in test_pred_sets:
            result = clf.clf.predict(test.data)
            print "Prediction is: {} expected {}".format(result, test.label)


def print_prediction(pred_labels, expected_index):
    for pred in pred_labels:
        print("Prediction: {} we expected {}").format(pred, expected_index)