#ifndef __ANN__
#define __ANN__

#include <FS.h>
#include "datastructures.h"
#include "parseEncog.h"

extern network ann;

void setup_neuralNetwork();
double sigmoid(double x);
networkResult _eval_network(network* ann);
networkResult EvaluateNetwork(group g);

#endif