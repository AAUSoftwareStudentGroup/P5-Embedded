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

#define _T 0
#define _X 1
#define _Y 2
#define _Z 3
#define _RX 4
#define _RY 5
#define _RZ 6

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
  int resultLength;
} networkResult;

typedef struct _network {
  // layers are ordered input layer first, output layer last
  layer* layers;
  int n_layers;
  networkResult lastResult;
} network;

typedef struct _group
{
  int length;
  datapoint* datapoints;
} group;

#endif
