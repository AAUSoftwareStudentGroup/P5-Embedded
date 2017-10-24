import requests
import json
from pandas.io.json import json_normalize
from Data_Classes.classes import TestSet, TrainSet, Label, ApiData, Property


def load_json():
    #r = requests.get('http://p5datahub.azurewebsites.net/api/modeltype/20/test')

    with open('testapi.json') as data_file:
        #d = json.loads(r.content)
        d = json.load(data_file)
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
            trainingSet = TrainSet(train['Id'])
            trainingSet.set_df(json_normalize(train['Data']))
            Api_data.add_trainSet(trainingSet)

        # read teastsets and transform it to dataframes
        for test in data['TestSet']:
            testSet = TestSet(test['Id'])
            testSet.set_df(json_normalize(test['Data']))
            Api_data.add_testSet(testSet)

    return Api_data