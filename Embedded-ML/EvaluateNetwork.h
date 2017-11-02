#ifndef __EVALUATE_NETWORK__
#define __EVALUATE_NETWORK__

#include <math.h>
#include "datastructures.h"

double sigmoid(double x);
layer* EvaluateNetwork(group g, network ann);

#endif