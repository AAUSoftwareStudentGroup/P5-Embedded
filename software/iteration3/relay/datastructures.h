#ifndef __DATASTRUCTURES__
#define __DATASTRUCTURES__

typedef struct _datapoint
{
  double T;
  double X;
  double Y;
  double Z;
  double RX;
  double RY;
  double RZ;
} datapoint;

typedef struct _group
{
  int length;
  datapoint* datapoints;
} group;

typedef struct _node {
  // there are as many weights as there are nodes in the next layer
  double* weights;
  double val;
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
  double* results;
  int length;
} networkResult;

typedef struct _network {
  // layers are ordered input layer first, output layer last
  layer* layers;
  int n_layers;
  networkResult lastResult;
  char** labels; // The number of labels is equal to the number of nodes in the final layer
} network;

#endif
