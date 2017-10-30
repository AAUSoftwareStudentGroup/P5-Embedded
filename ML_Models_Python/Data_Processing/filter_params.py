# this method is intended for extracting parameters into model_init and train parameters


def filter_params(argument, param_dict):
    argument = argument
    hidden_layers_arg = "hidden_layers"
    if hidden_layers_arg in param_dict:
        hidden_layers_string = param_dict[hidden_layers_arg]
        param_dict[hidden_layers_arg] = tuple(map(int, hidden_layers_string.split(",")))
    # todo this
    hidden_activations_arg = "hidden_activations"
    if hidden_activations_arg in param_dict:
        hidden_activations_string = param_dict[hidden_activations_arg]
        param_dict[hidden_activations_arg] = tuple(hidden_activations_string.split(","))

    if argument == "ann":
        nn_init_param_names = ["activation_function", "hidden_activations", "output_activation", "hidden_layers", "loss_function"]
        nn_learn_param_names = ["epochs", "batch_size"]

        return filter_specific_params(param_dict, nn_init_param_names, nn_learn_param_names)

    elif argument == "rnn":
        rnn_init_param_names = ["activation_function", "hidden_activations", "output_activation", "hidden_layers", "loss_function"]
        rnn_learn_param_names = ["epochs", "batch_size"]

        return filter_specific_params(param_dict, rnn_init_param_names, rnn_learn_param_names)

    elif argument == "svm":
        svm_init_param_names = ["C", "cache_size", "class_weight", "coef0", "decision_function_shape", "degree", "gamma", "kernel", "max_iter", 
                                "probability", "random_state", "shrinking", "tol", "verbose"]
        return filter_specific_params(param_dict, svm_init_param_names, None)

    elif (argument == "logistic_regression") or (argument == "lr"):
        lr_learn_param_names = ["epochs", "earlystopping"]
        return filter_specific_params(param_dict, None, lr_learn_param_names)


def filter_specific_params(param_dict, init_param, train_param):
    init_dict = {}
    train_dict = {}
    if init_param is not None:
        for i in xrange(len(init_param)):
            if init_param[i] in param_dict:
                init_dict[init_param[i]] = param_dict[init_param[i]]
    if train_param is not None:
        for j in xrange(len(train_param)):
            if train_param[j] in param_dict:
                train_dict[train_param[j]] = param_dict[train_param[j]]
    return init_dict, train_dict