#ifndef __DATASTRUCTURES__
#define __DATASTRUCTURES__

typedef struct _datapoint
{
  float X;
  float Y;
  float Z;
  float RX;
} datapoint;

typedef struct _group
{
  int length;
  datapoint* datapoints;
} group;

typedef struct _node {
  // there are as many weights as there are nodes in the next layer
  float* weights;
  float val; // sum of weighted inputs
  float out; // after activation function
  float error; // error value of a node (used during training)
  // int n_weights; 
} node;

typedef struct _layer {
  node* nodes;
  int n_nodes;
  node bias; // We assume that there is maximally one bias node per layer
  // node* biases;
  // int n_biases;
} layer;

typedef struct _networkResult {
  float* results;
  int length;
} networkResult;

typedef struct _network {
  // layers are ordered input layer first, output layer last
  layer* layers;
  int n_layers;
  networkResult lastResult;
  char** labels; // The number of labels is equal to the number of nodes in the final layer
} network;

// training example
typedef struct _example {
  group input;
  networkResult output;
} example;

#endif
