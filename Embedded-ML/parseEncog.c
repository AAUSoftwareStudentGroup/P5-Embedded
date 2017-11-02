#include "parseEncog.h"

// set file pointer `fp` to the position right after the seeked string
int _parseEncogModel_seekString(FILE* fp, char* searchString) {
	rewind(fp);
	char c;
	char* comparisonRefference = searchString;
	int found;
	while(feof(fp) == 0) {
		if(fgetc(fp) == *comparisonRefference) {
			if(*(++comparisonRefference) == '\0') {
				return 1;
			}
		} else comparisonRefference = searchString;
	}

	return 0;
}

void _parseEncogModel_parseNumberOfLayers(FILE* fp, network* n) {
	n->n_layers = 0;

	int tmp;
	_parseEncogModel_seekString(fp, "layerFeedCounts=");
	while(fscanf(fp, "%d,", &tmp) > 0)
		n->n_layers++;
	n->layers = malloc(sizeof(layer) * n->n_layers);
}

void _parseEncogModel_parseLayerSizes(FILE* fp, network* n) {
	int n_nodes;
	int layerIndex = n->n_layers-1; // start from last layer

	_parseEncogModel_seekString(fp, "layerFeedCounts=");
	while(fscanf(fp, "%d,", &n_nodes) > 0) {
		n->layers[layerIndex].n_nodes = n_nodes;
		n->layers[layerIndex].nodes = malloc(sizeof(node)*n_nodes);
		layerIndex--;
	}
}

void _parseEncogModel_allocateWeightArrays(network* ann) {
	int n_weights;
	layer* l;
	node* n;

	// for each layer except output
	for(l = ann->layers; l < ann->layers+ann->n_layers-1; l++) {
		// number of weights is the number of nodes in the next layer
		n_weights = (l+1)->n_nodes;

		l->bias.weights = malloc(sizeof(double) * n_weights);
		for(n = l->nodes; n < l->nodes+l->n_nodes; n++) {
			n->weights = malloc(sizeof(double) * n_weights);
		}
	}

	// handle output layer
	l->bias.weights = NULL;
	for(n = l->nodes; n < l->nodes+l->n_nodes; n++) {
		n->weights = NULL;
	}
}


void _parseEncogModel_parseWeights(FILE* fp, network* ann) {
	int n_weights, n_nodes;
	int weightIndex;
	layer* l;
	node* n;

	_parseEncogModel_seekString(fp, "weights=");

	// for each layer except output (output has no outgoing weights)
	for(l = ann->layers+ann->n_layers-2; l >= ann->layers; l--) {
		// number of weights is the number of nodes in the next layer
		n_weights = (l+1)->n_nodes;
		n_nodes = l->n_nodes;

		// itterate through the number of nodes in the next layer
		for(weightIndex = 0; weightIndex < n_weights; weightIndex++) {
			// for each node in the current layer
			for(n = l->nodes; n < l->nodes+n_nodes; n++) {
				// read a weight from the file
				fscanf(fp, "%lf,", &(n->weights[weightIndex]) );
			}

			// plus the bias node
			fscanf(fp, "%lf,", &(l->bias.weights[weightIndex]) );
		}
	}
}

network parseEncogModel(char* filePath) {
	FILE* fp = fopen(filePath, "r");
	network n;

	_parseEncogModel_parseNumberOfLayers(fp, &n);
	_parseEncogModel_parseLayerSizes(fp, &n);
	_parseEncogModel_allocateWeightArrays(&n);
	_parseEncogModel_parseWeights(fp, &n);

	return n;

}
