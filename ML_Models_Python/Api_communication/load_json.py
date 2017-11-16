import requests
import json
from pandas.io.json import json_normalize
from Data_Classes.classes import TestSet, TrainSet, ApiData, Property
import settings


def add_dataset(api_data, dataset, json_obj, cached_data_dict, dataset_type):
    if json_obj['Data'] is None:
        if dataset_type == 'training_set':
            api_data.add_trainSet(cached_data_dict[json_obj['Id']])
        else:
            api_data.add_testSet(cached_data_dict[json_obj['Id']])
    else:
        for group in json_obj['Data']:
            dataset.add_shot(json_normalize(group['Data']))
        if dataset_type == 'training_set':
            api_data.add_trainSet(dataset)
        else:
            api_data.add_testSet(dataset)


def load_json(modeltype_string):
    base_url = 'http://p5datahub.azurewebsites.net/api/modeltype/'
    base_cache_str = '?cachedIds='
    get_modeltype_url = base_url + modeltype_string + '/test'
    cached_data_ids = get_cached_data_string(settings.cached_data_dict)
    if not cached_data_ids == base_cache_str:
        get_modeltype_url = get_modeltype_url + cached_data_ids
    #r = requests.get(get_modeltype_url)
    r = requests.get("http://p5datahub.azurewebsites.net/api/model/154/test/120")
    print r
    d = json.loads(r.content)
    data = d['Data']
    if not (d['Success']) or not (d['ErrorKey'] == "NoError"):
        raise ValueError("Something wen't wrong in requesting data from server")
    Api_data = ApiData(data['Id'], data['ModelId'], data['ModelTypeId'], data['ModelTypeName'])
    # create properties
    [Api_data.set_parameter(Property(prop['Id'], prop['PropertyId'], prop['Name'], prop['Value'])) for prop in data["Parameters"]]

    # read trainsets and transform it to dataframes
    for train in data['TrainingSet']:
        label_name = train['Labels'][0]['Id']
        Api_data.add_label_name(label_name)
        trainingset = TrainSet(train['Id'])
        add_dataset(Api_data, trainingset, train, settings.cached_data_dict, "training_set")

    # read teastsets and transform it to dataframes
    for test in data['TestSet']:
        testset = TestSet(test['Id'])
        add_dataset(Api_data, testset, test, settings.cached_data_dict, "test_set")
    cache_data(Api_data, settings.cached_data_dict)
    return Api_data


def cache_data(api_data, cached_data_dict):
    for testSet in api_data.testSetData:
        cached_data_dict[testSet.id] = testSet

    for trainSet in api_data.trainSetData:
        cached_data_dict[trainSet.id] = trainSet


def get_cached_data_string(cached_data_dict):
    data_ids = ','.join(str(i) for i in cached_data_dict)
    cached_string = "?cachedIds="
    cached_string = cached_string + data_ids
    return cached_string