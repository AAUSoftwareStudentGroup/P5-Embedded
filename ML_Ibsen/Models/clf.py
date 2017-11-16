# try test
from keras.models import Sequential
from keras.layers import Dense, Dropout, LSTM
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
    def __init__(self, name, shape, output_shape, activation_function="sigmoid", hidden_activations=("sigmoid"), output_activation="softmax",
                 hidden_layers=(50), loss_function='categorical_crossentropy'):
        Clf_base.__init__(self, name)
        self.clf = Sequential()
        self.clf.add(Dense(input_dim=shape[0], output_dim=output_shape, activation=activation_function))
        # For a mean squared error regression problem
        if not isinstance(hidden_layers, int):
            for i in xrange(len(hidden_layers)):
                if len(hidden_layers) > len(hidden_activations):
                    raise ValueError("Plz specify an activation function for each hidden layer")
                elif len(hidden_layers) < len(hidden_activations):
                    raise ValueError("There must be a hidden layer for each hidden activation function") 
                self.clf.add(Dense(hidden_layers[i], activation=hidden_activations[i]))
        #self.clf.add(Dropout(0.4))
        self.clf.add(Dense(output_shape, activation=output_activation))
        optim = optimizers.RMSprop(lr = 0.01)
        self.clf.compile(optimizer=optim, loss=loss_function)

    def learn(self, train, labels, monitor="loss", epochs=1000, batch_size=2):
        # shuffle data in unison
        train, labels = shuffle(train, labels, random_state=5)
        early_stopping = EarlyStopping(monitor=monitor, patience=5, verbose=0, mode='auto')
        self.clf.fit(train, labels, batch_size=batch_size, epochs=epochs,
                     verbose=1, validation_split=0.2, callbacks=[early_stopping], validation_data=None, shuffle=True)


# keras docs https://keras.io/getting-started/faq/
# unit_forget_bias: Boolean. If True, add 1 to the bias of the forget gate at initialization. Setting it to true will also force bias_initializer="zeros". 
# TODO: make statefull RNN that can't be shuffled
class RNN(Clf_base):
    # make parameters here
    def __init__(self, name, shape, output_shape, activation_function="sigmoid", hidden_activations=("sigmoid"), output_activation="softmax",
                 hidden_layers=(50), loss_function='categorical_crossentropy', dropout=True, statefull=False):
        Clf_base.__init__(self, name)
        self.clf = Sequential()
        # get output length as parameter
        if not statefull:
            self.clf.add(LSTM(shape[1], input_shape=(shape), unit_forget_bias=True))
        else:
            # batch input takes batch size as first input
            self.clf.add(LSTM(shape[1], batch_input_shape=(1, shape[0], shape[1]), unit_forget_bias=True))
        if not isinstance(hidden_layers, int):
            for i in xrange(len(hidden_layers)):
                if len(hidden_layers) > len(hidden_activations):
                    raise ValueError("Plz specify an activation function for each hidden layer")
                elif len(hidden_layers) < len(hidden_activations):
                    raise ValueError("There must be a hidden layer for each hidden activation function") 
                self.clf.add(Dense(hidden_layers[i], activation=hidden_activations[i]))
        if dropout:
            self.clf.add(Dropout(0.1))
        self.clf.add(Dense(output_shape, activation=output_activation))
        optim = optimizers.RMSprop(lr=0.01)
        self.clf.compile(optimizer=optim, loss=loss_function,metrics=['accuracy'])

    def learn(self, train, labels, monitor="loss", epochs=1000, batch_size=2, statefull=False):
        # shuffle data in unison
        if not statefull:
            train, labels = shuffle(train, labels, random_state=5)
            early_stopping = EarlyStopping(monitor=monitor, patience=3, verbose=0, mode='auto')
            self.clf.fit(train, labels, epochs=epochs, batch_size=batch_size, validation_split=0.2, verbose=2, callbacks=[early_stopping], shuffle= True)
        else:
            for i in xrange(epochs):
                early_stopping = EarlyStopping(monitor=monitor, patience=3, verbose=0, mode='auto')
                self.clf.fit(train, labels, epochs=1, batch_size=1, verbose=2, callbacks=[early_stopping], shuffle= True)
                self.clf.reset_states()


class SVM(Clf_base):
    def __init__(self, name, C=25, cache_size=2000, class_weight=None, coef0=0.0, decision_function_shape='ovr', degree=3, gamma=0.1, kernel='sigmoid',
                 max_iter=5000, probability=True, random_state=None, shrinking=False, tol=0.00001, verbose=True, earlystopping = False):
        Clf_base.__init__(self, name)
        self.clf = svm.SVC(C=C, cache_size=cache_size, class_weight=class_weight, coef0=coef0, decision_function_shape=decision_function_shape,
                           degree=degree, gamma=gamma, kernel=kernel, max_iter=max_iter, probability=probability, random_state=random_state,
                           shrinking=shrinking, tol=float(tol), verbose=verbose)

    def learn(self, train, labels, monitor="loss"):
        train, labels = shuffle(train, labels, random_state=0)
        self.clf.fit(train, labels)


class LR(Clf_base):
    def __init__(self, name, shape, output_shape):
        Clf_base.__init__(self, name)
        self.clf = Sequential()
        self.clf.add(Dense(input_dim=shape[0], output_dim=output_shape, activation='softmax'))
        if output_shape == 2:
            self.clf.compile(optimizer='rmsprop', loss='binary_crossentropy')
        else:
            self.clf.compile(optimizer='rmsprop', loss='categorical_crossentropy')

    def learn(self, train, labels, epochs=1000, monitor="loss", earlystopping=False):
        train, labels = shuffle(train, labels, random_state=0)
        if earlystopping:
            early_stopping = EarlyStopping(monitor=monitor, patience=3, verbose=0, mode='auto') 
            self.clf.fit(train, labels, validation_split=0.2, epoch=epochs, callbacks=[early_stopping])
        else:
            self.clf.fit(train, labels, epochs=epochs)


def create_classifier(argument, parameters, input_shape, output_shape):
    # problem rnn is initilised eventhough it is not called
    argument = argument.lower()
    if argument == "ann":
        return ANN(argument, input_shape, output_shape, **parameters)
    elif argument == "rnn":
        return RNN("RNN", input_shape, output_shape, **parameters)
    elif argument == "svm":
        return SVM("SVM", **parameters)
    elif argument == "lr":
        return LR("LR", input_shape, output_shape)