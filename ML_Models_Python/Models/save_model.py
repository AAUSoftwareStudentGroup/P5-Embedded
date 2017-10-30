from sklearn.externals import joblib
from keras.models import load_model


def sklearn_save_model(model, filepath):
    if ".pkl" not in filepath:
        joblib.dump(model, "{}.pkl".format(filepath))
    else:
        joblib.dump(model, filepath)


def keras_save_model(model, filepath):
    if ".h5" not in filepath:
        model.save("{}.h5".format(filepath))
    else:
        model.save(filepath)


def sklearn_load_model(filepath):
    return joblib.load(filepath)


def keras_load_model(filepath):
    return load_model(filepath)


def execute_save_method(model_name, model, filepath):
    mappings = {
        "keras": keras_save_model,
        "sklearn": sklearn_save_model
    }
    if model_name not in mappings:
        raise ValueError("Selected Method for saving does not exist")
    # Get the function from switcher dictionary
    func = mappings.get(model_name, lambda: "nothing")
    # Execute the function
    return func(model, filepath)


def execute_load_method(model_name, filepath):
    mappings = {
        "keras": keras_load_model,
        "sklearn": sklearn_load_model
    }
    if model_name not in mappings:
        raise ValueError("Selected Method for loading does not exist")
    func = mappings.get(model_name, lambda: "nothign")
    return func(filepath)