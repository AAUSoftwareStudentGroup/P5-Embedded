# this method is intended for extracting parameters into model_init and train parameters
# currently this is written for NN's
# if more then just add a flag and give an extra parameter


def filter_params(argument, parameters_dict):

    if (argument == "ann") or (argument == "rnn"):
        nn_init_param_names = ["activation_function", "hidden_activations", "output_activation", "hidden_layers", "optimizer", "loss_function"]
        nn_train_param_names = ["monitor", "epochs", "batch_size"]

        nn_init_dict = {}
        train_init_dict = {}

        for i in xrange(len(nn_init_param_names)):
            if nn_init_param_names[i] in parameters_dict:
                nn_init_dict[nn_init_param_names[i]] = parameters_dict[nn_init_param_names[i]]

        for j in xrange(len(nn_train_param_names)):
            if nn_train_param_names[j] in parameters_dict:
                train_init_dict[nn_train_param_names[j]] = parameters_dict[nn_train_param_names[j]]

        return nn_init_dict, train_init_dict
    elif argument == "SVM":
        svm_init_param_names = ["C", "cache_size", "class_weight", "coef0","decision_function_shape", "degree", "gamma", "kernel", "max_iter", 
                                "probability", "random_state", "shrinking", "tol", "verbose"]
        svm_init_dict = {}
        for i in xrange(len(svm_init_param_names)):
            if svm_init_param_names[i] in parameters_dict:
                svm_init_dict[svm_init_param_names[i]] = parameters_dict[svm_init_param_names[i]]
        return svm_init_dict, None