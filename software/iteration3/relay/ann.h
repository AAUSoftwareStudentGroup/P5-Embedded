#ifndef __ANN__
#define __ANN__

#include <FS.h>
#include "datastructures.h"
// #include "parseEncog.h"
#include "config.h"

inline float sigmoid(float x);
inline float derivedSigmoid(float x);
networkResult EvaluateNetwork(network* ann, group g);
network initiateRandomNetwork(char** labels, int n_labels);
void deallocateNetwork(network* n);
bool _calculateOutputError(network* n, networkResult expectedOutput);
void _backpropogateErrorValues(network* n);
void _updateWeights(network* n, float learningRate);
networkResult trainNetwork(network* n, example* examples, int n_examples);

#endif