class DataSet:

    def __init__(self, id):
        self.id = id
        self.shots = []

    def add_shot(self, shot_data):
        # rename columns to the pythonic way
        shot_data.rename(columns={'Time': 'time', 'X': 'x', 'Y': 'y', 'Z': 'z', 'RX': 'rx', 'RY': 'ry', 'RZ': 'rz'},
                         inplace=True)
        self.shots.append(shot_data)       


class TestSet(DataSet):
    def __init__(self, id):
        DataSet.__init__(self, id)


class TrainSet(DataSet):
    def __init__(self, id):
        DataSet.__init__(self, id)


class LabelKind:
    Ordinary, Arbitrary, = range(2)


class Label:

    def __init__(self, id, confidence):
        self.id = id
        self.confidence = confidence


class ApiData:

    def __init__(self, id, model_id, model_type_id, model_type_name):
        # set this with parameter in constructor when API is done
        self.model_type_name = model_type_name
        self.id = id
        self.model_id = model_id
        self.model_type_id = model_type_id
        self.parameters = {}
        self.trainSetData = []
        self.testSetData = []
        self.label_names = []

    def set_parameter(self, prop):
        self.parameters[prop.name] = prop.value

    def add_trainSet(self, trainSet):
        self.trainSetData.append(trainSet)

    def add_testSet(self, testSet):
        self.testSetData.append(testSet)

    def add_label_name(self, name):
        self.label_names.append(name)


class Property:

    def __init__(self, id, property_id, name, value):
        self.id = id
        self.property_id = property_id
        self.name = name
        self.value = value