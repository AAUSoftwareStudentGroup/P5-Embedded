#ifndef __PARSE_ENCOG__
#define __PARSE_ENCOG__

#include <stdio.h>
#include <stdlib.h>

// example of usage:
// network ann = parseEncogModel("Test-120-Model-128-encog.txt");


typedef struct _node {
	// there are as many weights as there are nodes in the next layer
	double* weights;
	// int n_weights; 
} node;

typedef struct _layer {
	node* nodes;
	int n_nodes;
	node bias; // We assume that there is maximally one bias node per layer
	// node* biases;
	// int n_biases;
} layer;

typedef struct _network {
	// layers are ordered input layer first, output layer last
	layer* layers;
	int n_layers;
} network;

int _parseEncogModel_seekString(FILE* fp, char* searchString);
void _parseEncogModel_parseNumberOfLayers(FILE* fp, network* n);
void _parseEncogModel_parseLayerSizes(FILE* fp, network* n);
void _parseEncogModel_allocateWeightArrays(network* ann);
void _parseEncogModel_parseWeights(FILE* fp, network* ann);

#endif