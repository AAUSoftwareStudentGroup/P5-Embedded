#include "ann.h"

network ann;

void setup_neuralNetwork() {
  parseEncogModel_setup();
  ann = parseEncogModel();
}

double sigmoid(double x) {
  double returnValue, expValue;

  expValue = exp((double) -x);
  returnValue = 1.0l / (1.0l + expValue);

  return returnValue;
}

networkResult _eval_network(network* ann) {
  node* current;   
  node* next;
  layer* l;
  int nextNodeIndex;

  // init node values to zero
  for(l = ann->layers+1; l < ann->layers + ann->n_layers; l++) {
    for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
      current->val = 0;
    }
  }

  for(l = ann->layers; l < ann->layers + ann->n_layers-1; l++) {
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

  /*for(l = ann->layers; l < ann->layers + ann->n_layers; l++)
    for(current = l->nodes; current < l->nodes+l->n_nodes; current++)
      Serial.println("next->val: Layer " + String(l - ann->layers) + ": Node " + String(current - l->nodes) + ": " + String(current->val, 14));
  for(l = ann->layers; l < ann->layers + ann->n_layers; l++)
    for(current = l->nodes; current < l->nodes+l->n_nodes; current++)
      for(nextNodeIndex = 0; nextNodeIndex < (l+1)->n_nodes; nextNodeIndex++)
        Serial.println("Weights: Layer " + String(l - ann->layers) + ": Node " + String(current - l->nodes) + "->" + String(nextNodeIndex) + ": " + String(current->weights[nextNodeIndex], 14));
  */
  // copy final layer node values to result vector
  l = ann->layers + ann->n_layers-1;
  for(int i = 0; i < l->n_nodes; i++) {
    ann->lastResult.results[i] = l->nodes[i].val;
  }
  return ann->lastResult;
}

networkResult EvaluateNetwork(group g) {
  // Put group into network. Assumes network has enough nodes to hold input
  for(int i = 0; i < 10; i++) {
    ann.layers[0].nodes[i*4 + 0].val = g.datapoints[i].X;
    ann.layers[0].nodes[i*4 + 1].val = g.datapoints[i].Y;
    ann.layers[0].nodes[i*4 + 2].val = g.datapoints[i].Z;
    ann.layers[0].nodes[i*4 + 3].val = g.datapoints[i].RX;
  }

  // Evaluate network and return result
  return  _eval_network(&ann);
}