# try test
from keras.models import Sequential
from keras.layers import Dense, Dropout, LSTM
from keras import optimizers
from keras.callbacks import EarlyStopping
from sklearn import svm
from sklearn.utils import shuffle
from Data_Processing.history_plot import plot_history


def create_classifier_helper(model_name, init_param, input_shape=None, output_shape=None):
    clf = create_classifier(model_name, init_param, input_shape, output_shape)
    return clf


def teach_model(clf, shots, labels, learn_param):
    clf.learn(shots, labels, **learn_param)


def create_classifier(argument, parameters, input_shape, output_shape):
    argument = argument.lower()
    if argument == "ann":
        return ANN(argument, input_shape, output_shape, **parameters)
    elif argument == "rnn":
        return RNN("RNN", input_shape, output_shape, **parameters)
    elif argument == "svm":
        return SVM("SVM", **parameters)
    elif argument == "lr":
        return LR("LR", input_shape, output_shape)
    elif argument == "rnns":
        return SimpleRNN("rnns", input_shape, output_shape, **parameters)
    elif argument == "stackedrnn":
        return StackedRNN("stackedrnn", input_shape, output_shape, **parameters)
    elif argument == "simplernn":
        return SimpleRNN("SimpleRNN", input_shape, output_shape, **parameters)


class ClfBase(object):
    def __init__(self, name):
        self.name = name

    def train(shots, labels, init_param, learn_param):
        clf = create_classifier_helper("ANN", init_param, shots.shape[1:], labels.shape[1])
        teach_model(clf, shots, labels, learn_param)
        return clf


class ScikitBase(ClfBase):

    def __init__(self, name):
        super(ScikitBase, self).__init__(name)

    def learn(self, train, labels):
        train, labels = shuffle(train, labels, random_state=0)
        self.clf.fit(train, labels)


class KerasBase(ClfBase):

    def __init__(self, name):
        super(KerasBase, self).__init__(name)

    def learn(self, train, labels, monitor="loss", epochs=1000, batch_size=10):
        train, labels = shuffle(train, labels, random_state=7)
        early_stopping = EarlyStopping(monitor=monitor, patience=10, verbose=0, mode='auto')
        history = self.clf.fit(train, labels, epochs=int(epochs), batch_size=int(batch_size), validation_split=0.2, verbose=2, callbacks=[early_stopping], shuffle= True)
        plot_history(history)

    def predict(self, shots, labels, init_param, learn_param, name):
        clf = create_classifier_helper(name, init_param, shots.shape[1:], labels.shape[1])
        teach_model(clf, shots, labels, learn_param)


class ANN(KerasBase):
    def __init__(self, name, shape, output_shape, activation_function="sigmoid", hidden_activations=("sigmoid"), output_activation="softmax",
                 hidden_layers=(50), loss_function='categorical_crossentropy'):
        super(KerasBase, self).__init__(name)
        self.clf = Sequential()
        self.clf.add(Dense(input_shape=shape[0], output_dim=output_shape, activation=activation_function))
        # For a mean squared error regression problem
        if not isinstance(hidden_layers, int):
            for i in xrange(len(hidden_layers)):
                if len(hidden_layers) > len(hidden_activations):
                    raise ValueError("Plz specify an activation function for each hidden layer")
                elif len(hidden_layers) < len(hidden_activations):
                    raise ValueError("There must be a hidden layer for each hidden activation function") 
                self.clf.add(Dense(hidden_layers[i].strip(), activation=str(hidden_activations[i])).strip())
        #self.clf.add(Dropout(0.4))
        self.clf.add(Dense(output_shape, activation=output_activation))
        optim = optimizers.RMSprop(lr = 0.01)
        self.clf.compile(optimizer=optim, loss=loss_function)

    def predict(self, shots, labels, init_param, learn_param):
        super(ANN, self).predict(shots, labels, init_param, learn_param, "ANN")


class RNN(KerasBase):
    def __init__(self, name, shape, output_shape, activation_function="relu", hidden_activations=("relu"), output_activation="softmax",
                 hidden_layers=(50), loss_function='categorical_crossentropy', dropout=True):
        super(KerasBase, self).__init__(name)
        self.clf = Sequential()
        if not isinstance(hidden_layers,(int, long)):
            self.clf.add(LSTM(hidden_layers[0], input_shape=(shape), unit_forget_bias=True, use_bias=True, activation=activation_function,
                              bias_initializer='zeros'))
            for i in xrange(len(hidden_layers)):
                if len(hidden_layers) > len(hidden_activations):
                    raise ValueError("Plz specify an activation function for each hidden layer")
                elif len(hidden_layers) < len(hidden_activations):
                    raise ValueError("There must be a hidden layer for each hidden activation function")
                self.clf.add(Dense(hidden_layers[i], activation=str(hidden_activations[i]).strip()))
        else:
            self.clf.add(LSTM(hidden_layers, input_shape=(shape), unit_forget_bias=True, use_bias=True,
                              bias_initializer='zeros'))
        # output layer
        self.clf.add(Dense(output_shape, activation=output_activation))
        # solver / optimizer
        optim = optimizers.RMSprop(lr=0.01)
        self.clf.compile(optimizer=optim, loss=loss_function, metrics=['accuracy'])
        print self.clf.summary()

    def predict(self, shots, labels, init_param, learn_param, name):
        super(ANN, self).predict(shots, labels, init_param, learn_param, "RNN")


class StackedRNN(KerasBase):
    def __init__(self, name, shape, output_shape, activation_function="relu", hidden_activations=("relu"), output_activation="softmax",
                 hidden_layers=(50), loss_function='categorical_crossentropy', dropout=True):
        super(KerasBase, self).__init__(name)

        self.clf = Sequential()
        self.clf.add(LSTM(hidden_layers[0], input_shape=(shape), unit_forget_bias=True, use_bias=True, bias_initializer='zeros', return_sequences=True))

        for i in xrange(len(hidden_layers)):
            if len(hidden_layers) > len(hidden_activations):
                raise ValueError("Plz specify an activation function for each hidden layer")
            elif len(hidden_layers) < len(hidden_activations):
                raise ValueError("There must be a hidden layer for each hidden activation function")
            if not i == len(hidden_layers ) - 1:
                self.clf.add(LSTM(hidden_layers[0], input_shape=(shape), unit_forget_bias=True, use_bias=True, bias_initializer='zeros', return_sequences=True))
            else:
                self.clf.add(LSTM(hidden_layers[0], input_shape=(shape), unit_forget_bias=True, use_bias=True,
                                  bias_initializer='zeros', return_sequences=False))

        self.clf.add(Dense(output_shape, activation=output_activation))
        optim = optimizers.RMSprop(lr=0.01)
        self.clf.compile(optimizer=optim, loss=loss_function, metrics=['accuracy'])

        print self.clf.summary()

    def predict(self, shots, labels, init_param, learn_param, name):
        super(ANN, self).predict(shots, labels, init_param, learn_param, "RNN")


class SimpleRNN(KerasBase):
    def __init__(self, name, shape, output_shape, HL_neurons =None, activation_function="relu", output_activation="softmax", loss_function='categorical_crossentropy'):
        super(KerasBase, self).__init__(name)
        if HL_neurons is None:
            HL_neurons = output_shape
        self.clf = Sequential()
        self.clf.add(LSTM(int(HL_neurons), input_shape=(shape), activation=activation_function, unit_forget_bias=True, use_bias=True,
                          bias_initializer='zeros'))
        self.clf.add(Dense(output_shape, activation=output_activation))
        optim = optimizers.RMSprop(lr=0.01)
        self.clf.compile(optimizer=optim, loss=loss_function, metrics=['accuracy'])

    def predict(self, shots, labels, init_param, learn_param, name):
        super(ANN, self).predict(shots, labels, init_param, learn_param, "SimpleRNN")


class LR(KerasBase):
    def __init__(self, name, shape, output_shape):
        super(KerasBase, self).__init__(name)
        self.clf = Sequential()
        self.clf.add(Dense(input_dim=shape[0], output_dim=output_shape, activation='softmax'))
        self.clf.compile(optimizer='rmsprop', loss='categorical_crossentropy', metrics=['accuracy'])

    def predict(self, shots, labels, init_param, learn_param, name):
        super(ANN, self).predict(shots, labels, init_param, learn_param, "LR")


class SVM(ScikitBase):
    def __init__(self, name, C=25, cache_size=2000, class_weight=None, coef0=0.0, decision_function_shape='ovr', degree=3, gamma=0.1, kernel='sigmoid',
                 max_iter=5000, probability=True, random_state=None, shrinking=False, tol=0.00001, verbose=True, earlystopping = False):
        super(ScikitBase, self).__init__(name)
        self.clf = svm.SVC(C=C, cache_size=cache_size, class_weight=class_weight, coef0=coef0, decision_function_shape=decision_function_shape,
                           degree=degree, gamma=gamma, kernel=kernel, max_iter=max_iter, probability=probability, random_state=random_state,
                           shrinking=shrinking, tol=float(tol), verbose=verbose)

    def predict(shots, labels, init_param, learn_param):
        clf = create_classifier_helper("SVM", init_param)
        teach_model(clf, shots, labels, learn_param)

