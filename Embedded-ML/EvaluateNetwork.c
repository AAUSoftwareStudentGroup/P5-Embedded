#include "EvaluateNetwork.h"

double sigmoid(double x){
    double returnValue;
    double expValue;
    
    expValue = exp((double) -x);
    
    returnValue = 1 / (1 + expValue);
    
    return returnValue;
}

layer* EvaluateNetwork(group g, network ann) {
	// fill input layer
	layer* l = ann.layers;
	node* current; 
	node* next;
	int nextNodeIndex;

	for(int i = 0; i < 10; i++) {
		l->nodes[i*4 + 0].val = g.datapoints[i].X;
		l->nodes[i*4 + 1].val = g.datapoints[i].Y;
		l->nodes[i*4 + 2].val = g.datapoints[i].Z;
		l->nodes[i*4 + 3].val = g.datapoints[i].RX;
	}

	// init node values to zero
	for(l = ann.layers+1; l < ann.layers + ann.n_layers; l++) {
		for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
			current->val = 0;
		}
	}
	
	// loop through layers
	for(l = ann.layers; l < ann.layers + ann.n_layers-1; l++) {
		for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
			for(nextNodeIndex = 0, next = (l+1)->nodes; nextNodeIndex < (l+1)->n_nodes; nextNodeIndex++, next++) {
				next->val += current->val*current->weights[nextNodeIndex];
			}
		}
		for(nextNodeIndex = 0, next = (l+1)->nodes; nextNodeIndex < (l+1)->n_nodes; nextNodeIndex++, next++) {
			next->val += l->bias.weights[nextNodeIndex];
			next->val = sigmoid(next->val);
		}
	}

	// return result
	return l;
}
