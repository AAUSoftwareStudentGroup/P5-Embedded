import json
import requests
from Models.train import get_features_from_shots
from Models.predict import predict_tests
from Models.save_model import execute_save_method
from Models.test_helpers import create_tests
from Models.strategy import Strategy
from Models.create_models import ModelCreations
from Api_communication.generate_description import generate_description
from Api_communication.load_json import load_json
from Data_Classes.classes import LabelKind
from Data_Processing.generate_labels import generate_labels
from Data_Processing.filter_params import filter_params
from keras.utils import plot_model

import sys


def extract_weights(clf):
    count = 0
    for layer in clf.layers:
        weights = layer.get_weights()
        for w in weights:
            print w.size
            count = count + w.size
    print "count is: {}".format(count)
    return count


def get_ml_func(argument):
    modelCreations = ModelCreations()
    mappings = {
        "ann": modelCreations.create_ANN,
        "rnn": modelCreations.create_RNN,
        "svm": modelCreations.create_SVM,
        "lr": modelCreations.create_LR,
        "rnns": modelCreations.create_RNNS,
        "simplernn": modelCreations.create_SimpleRNN,
        "stackedrnn": modelCreations.create_StackedRNN
    }
    if argument not in mappings:
        raise ValueError("Selected Method for Feature Extraction does not exist")
    # Get the function from switcher dictionary
    return mappings.get(argument, lambda: "nothing")


def identifyPlayer(modeltype_str, send=False, save=False, visualize_model=False):
    data = load_json(modeltype_str)
    labels = data.label_names

    # read this in load json at a later point
    model_name = data.model_type_name.split()[0].lower()

    if model_name == "":
        raise "ValueError: Model name cannot be blank"
    elif model_name == "logistic_regression":
        model_name = "lr"
    model_dict = {'ann': LabelKind.Arbitrary, 'rnn': LabelKind.Arbitrary, 'rnns': LabelKind.Arbitrary, 'simplernn': LabelKind.Arbitrary, 'svm': LabelKind.Ordinary, 'lr': LabelKind.Arbitrary
                  , 'stackedrnn': LabelKind.Arbitrary}

    new_labels, unique_labels = generate_labels(labels, model_dict[model_name])

    result = get_features_from_shots(data.trainSetData, new_labels, model_name)

    tests = data.testSetData

    ml_func = get_ml_func(model_name)

    ml_strat = Strategy(ml_func)

    init_params = {}
    train_params = {}
    # split params into those for training and those for fitting
    if bool(data.parameters):
        init_params, train_params = filter_params(model_name, data.parameters)
    clf = ml_strat.execute(result[0], result[1], init_params, train_params)

    tests_data = create_tests(tests, model_name)

    preds = None
    # contains the probability for each label
    preds = predict_tests(clf.clf, tests_data, unique_labels, model_dict[model_name])

    description = generate_description(model_name, data.parameters)

    data_set_results = []
    for i, pred in enumerate(preds):
        classification_data = [{"LabelId": l.id, "Confidence": float(l.confidence)} for l in pred]
        data_set_result = {"DataSetId": tests[i].id, "Classifications": classification_data}
        data_set_results.append(data_set_result)

    response = json.dumps({"Id": data.id, "ModelId": data.model_id, "DataSetResults": data_set_results, "Author":"Ibsen", "Description":description})

    print (response)

    extract_weights(clf.clf)

    name = "{}_{}_{}".format(data.model_type_name, data.id, data.model_id)
    if save:
        if (model_name == 'ann') or (model_name == 'rnn'):
            execute_save_method("keras", clf.clf, name)
            print ("model saved")
            sys.exit()
        elif model_name == 'svm':
            execute_save_method("sklearn", clf.clf, name)

    if visualize_model:

        plot_model(clf.clf, to_file="{model_name}.png", show_layer_names=False, show_shapes=False)
        print ("model has been ")
        print ("plz terminate")
        sys.exit()

    if send:
        headers = {'content-type': 'application/json'}
        post_response = requests.post("https://p5datahub.azurewebsites.net/api/test/{}/result".format(data.id), data= response, headers=headers)
        print (post_response)