#ifndef __ANN__
#define __ANN__

#include <FS.h>
#include "datastructures.h"
#include "parseEncog.h"
#include "config.h"

void setup_neuralNetwork();
inline double sigmoid(double x);
networkResult EvaluateNetwork(network* ann, group g);
network initiateRandomNetwork();
void deallocateNetwork(network* n);
bool _calculateOutputError(network* n, networkResult expectedOutput);
void _backpropogateErrorValues(network* n);
void _updateWeights(network* n, double learningRate);
void trainNetwork(network* n, example* examples, int n_examples);

#endif