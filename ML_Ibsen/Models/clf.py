# try test
from sklearn.neural_network import MLPClassifier
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers import Activation
from keras import optimizers
from keras.callbacks import EarlyStopping
from sklearn import svm
from sklearn.utils import shuffle


class Clf_base:
    def __init__(self, name):
        self.name = name


# TODO find a way to specify learning rate
class ANN(Clf_base):
    # make parameters here
    def __init__(self, name, shape, output_shape, activation_function="sigmoid", hidden_activations=("sigmoid"), output_activation="sigmoid",
                 hidden_layers=(24), optimizer="Nadam", loss_function='mse', lr= 0.001):
        Clf_base.__init__(self, name)
        self.clf = Sequential([
                              Dense(shape[0], input_shape=(shape)),
                              Activation(activation_function),
                              ])
        # For a mean squared error regression problem
        if not isinstance(hidden_layers, int):
            for i in xrange(len(hidden_layers)):
                if len(hidden_layers) > len(hidden_activations):
                    raise ValueError("Plz specify an activation function for each hidden layer")
                elif len(hidden_layers) < len(hidden_activations):
                    raise ValueError("There must be a hidden layer for each hidden activation function") 
                self.clf.add(Dense(hidden_layers[i], activation=hidden_activations[i]))

        self.clf.add(Dense(output_shape, activation=output_activation))

        self.clf.compile(optimizer=optimizer, loss=loss_function, metrics=['accuracy'])

    def learn(self, train, labels, monitor="val_loss", epochs=2000, batch_size=4):
        # shuffle data in unison
        train, labels = shuffle(train, labels, random_state=0)
        early_stopping = EarlyStopping(monitor='val_loss', patience=5, verbose=0, mode='auto')
        self.clf.fit(train, labels, batch_size=batch_size, epochs=epochs,
                     verbose=1, callbacks=[early_stopping],
                     validation_split=0.25, validation_data=None, shuffle=True)
        return self.clf


# keras docs https://keras.io/getting-started/faq/
# unit_forget_bias: Boolean. If True, add 1 to the bias of the forget gate at initialization. Setting it to true will also force bias_initializer="zeros". 
# TODO: figure out if each node requires lstm
class RNN(Clf_base):
    # make parameters here
    def __init__(self, name, shape, output_shape, activation_function="sigmoid", hidden_activations=("sigmoid"), output_activation="sigmoid",
                 hidden_layers=(24), optimizer="adam", loss_function='mse', lr=0.001):
        Clf_base.__init__(self, name)
        self.clf = Sequential()
        # get output length as parameter
        self.clf.add(LSTM(shape[1], input_shape=(shape), unit_forget_bias=True))
        if not isinstance(hidden_layers, int):
            for i in xrange(len(hidden_layers)):
                if len(hidden_layers) > len(hidden_activations):
                    raise ValueError("Plz specify an activation function for each hidden layer")
                elif len(hidden_layers) < len(hidden_activations):
                    raise ValueError("There must be a hidden layer for each hidden activation function") 
                self.clf.add(Dense(hidden_layers[i], activation=hidden_activations[i]))
        # get output length as parameter
        self.clf.add(Dense(output_shape, activation=activation_function))
        self.clf.compile(optimizer=optimizer, loss=loss_function, metrics=['accuracy'])

    def learn(self, train, labels, monitor="val_loss", epochs=2000, batch_size=4):
        # shuffle data in unison
        train, labels = shuffle(train, labels, random_state=0)
        early_stopping = EarlyStopping(monitor='val_loss', patience=5, verbose=0, mode='auto')
        self.clf.fit(train, labels, epochs=epochs, batch_size=batch_size, validation_split=0.3, verbose=2, callbacks=[early_stopping], shuffle= False)
        return self.clf


class SVM(Clf_base):
    def __init__(self, name, C=10000000, cache_size=200, class_weight=None, coef0=0.1, decision_function_shape='ovr', degree=3, gamma=1e-8, kernel='rbf',
                 max_iter=-1, probability=True, random_state=None, shrinking=True, tol=0.001, verbose=True):
        self.clf = svm.SVC(C=C, cache_size=cache_size, class_weight=class_weight, coef0=coef0, decision_function_shape=decision_function_shape,
                           degree=degree, gamma=gamma, kernel=kernel, max_iter=max_iter, probability=probability, random_state=random_state,
                           shrinking=shrinking, tol=tol, verbose=verbose)

    def learn(self, train, labels):
        self.clf.fit(train, labels)
        return self.clf


def create_classifier(argument, input_shape, output_shape, parameters,):
    # problem rnn is initilised eventhough it is not called
    argument = argument.lower()
    if argument == "ann":
        return ANN(argument, input_shape, output_shape, **parameters)
    elif argument == "rnn":
        return RNN("RNN", input_shape, output_shape, **parameters)
    if argument == "svm":
        return SVM("SVM", **parameters)