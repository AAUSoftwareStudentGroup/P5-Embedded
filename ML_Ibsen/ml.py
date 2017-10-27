import json
import requests
from Models.train import get_features_from_shots
from Models.save_model import execute_save_method
from Models.test_helpers import create_tests
from Models.strategy import Strategy
from Predict.predict import ANN_predict, RNN_predict, SVM_predict, LR_predict, predict_tests
from Api_communication.generate_description import generate_description
from Api_communication.load_json import load_json
from Data_Classes.classes import LabelKind
from Data_Processing.generate_labels import generate_labels
from Data_Processing.filter_params import filter_params


def get_ml_func(argument):
    mappings = {
        "ann": ANN_predict,
        "rnn": RNN_predict,
        "svm": SVM_predict,
        "lr": LR_predict
    }
    if argument not in mappings:
        raise ValueError("Selected Method for Feature Extraction does not exist")
    # Get the function from switcher dictionary
    return mappings.get(argument, lambda: "nothing")


def IdentifyPlayer():
    if not __name__ == "__main__":
        raise ValueError("This method should only be called from main")

    data = load_json()
    labels = data.label_names

    # read this in load json at a later point
    model_name = data.model_type_name.lower()

    if model_name == "":
        raise "ValueError: does not know this model type name"
    elif model_name == "logistic_regression":
        model_name == "lr"
    model_dict = {'ann': LabelKind.Arbitrary, 'rnn': LabelKind.Arbitrary, 'svm': LabelKind.Ordinary, 'lr': LabelKind.Arbitrary}

    new_labels, unique_labels = generate_labels(labels, model_dict[model_name])

    result = get_features_from_shots(data.trainSetData, new_labels)

    tests = data.testSetData

    ml_func = get_ml_func(model_name)

    ml_strat = Strategy(ml_func)

    # split params into those for training and those for fitting
    init_params, train_params = filter_params(model_name, data.parameters)
    clf = ml_strat.execute(result[0], result[1], init_params, train_params)

    # process the accelerometer and gyroscope data and split into movements
    tests_data = create_tests(tests)

    # contains the probability for each person
    preds = None

    preds = predict_tests(clf.clf, tests_data, model_name, unique_labels, model_dict[model_name])

    description = generate_description(model_name, data.parameters)

    data_set_results = []
    for i, pred in enumerate(preds):
        classification_data = [{"LabelId": l.id, "Confidence": float(l.confidence)} for l in pred]
        data_set_result = {"DataSetId": tests[i].id, "Classifications": classification_data}
        data_set_results.append(data_set_result)

    response = json.dumps({"Id": data.id, "ModelId": data.model_id, "DataSetResults": data_set_results, "Author":"Ibsen", "Description":description})

    print response

    save = False

    name = "{}_{}_{}.pkl".format(data.model_type_name, data.id, data.model_id)
    if save:
        if (model_name == 'ann') or (model_name == 'rnn'):
            execute_save_method("keras", clf, name)
        elif model_name == 'svm':
            execute_save_method("sklearn", clf, name)

    to_send = raw_input("send request? (y/n)")

    if to_send == "y":
        headers = {'content-type': 'application/json'}
        post_response = requests.post("https://p5datahub.azurewebsites.net/api/test/{}/result".format(data.id), data= response, headers=headers)
        print post_response


#this should be set up elsewhere but ok here for now

IdentifyPlayer()