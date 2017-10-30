# make a script allowing to laod a stored model:
import sys
from Models.save_model import execute_load_method

def load_model():
    argv = sys.argv
    if len(argv) is not 3:
        raise ValueError("load model, must take twp parameters: model_type e.g. keras, and filepath")
    # model_type, filepath
    model = execute_load_method(argv[1], [argv[2]])
    return model
