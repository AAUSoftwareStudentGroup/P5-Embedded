import requests
import json
from pandas.io.json import json_normalize
from Data_Classes.classes import TestSet, TrainSet, ApiData, Property


def add_shots(dataset, json_obj):
    for group in json_obj['Data']:
        dataset.add_shot(json_normalize(group['Group']))

def load_json():
    r = requests.get('http://p5datahub.azurewebsites.net/api/model/124/test/120')

    #with open('groupshotsapi.json') as data_file:
    d = json.loads(r.content)
        #d = json.load(data_file)
    data = d['Data']
    if not (d['Success']) or not (d['ErrorKey'] == "NoError"):
        raise ValueError("Something wen't wrong in requesting data from server")
    Api_data = ApiData(data['Id'], data['ModelId'], data['ModelTypeId'], data['ModelTypeName'])
    # create properties
    [Api_data.set_parameter(Property(prop['Id'], prop['PropertyId'], prop['Name'], prop['Value'])) for prop in data["Parameters"]]

    # read trainsets and transform it to dataframes
    for train in data['TrainingSet']:
        print "hej"
        label_name = train['Labels'][0]['Id']
        Api_data.add_label_name(label_name)
        trainingset = TrainSet(train['Id'])
        add_shots(trainingset, train)
        Api_data.add_trainSet(trainingset)

    # read teastsets and transform it to dataframes
    for test in data['TestSet']:
        testset = TestSet(test['Id'])
        add_shots(testset, test)
        Api_data.add_testSet(testset)
    return Api_data