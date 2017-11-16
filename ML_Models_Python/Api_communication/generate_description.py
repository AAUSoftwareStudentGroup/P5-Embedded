def generate_description(model_name, parameters, author="Ibsen"):
    param_list = ["Parameters given to model: \n"]
    for param in parameters:
        param_list.append("(name:{}, value:{}) ".format(param, parameters[param]))

    param_string = ''.join(param_list)

    return "Model used: {} with parameters {} \nWritten by {}".format(model_name, param_string, author)