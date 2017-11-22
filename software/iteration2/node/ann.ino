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

networkResult EvaluateNetwork(group g) {
  layer* l;
  node* current; 
  node* next;
  int nextNodeIndex;

  // init node values to zero
  for(l = ann.layers; l < ann.layers + ann.n_layers; l++) {
    for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
      current->val = 0;
    }
  }

  l = ann.layers;
  for(int i = 0; i < 10; i++) {
    l->nodes[i*4 + 0].val = g.datapoints[i].X;
    l->nodes[i*4 + 1].val = g.datapoints[i].Y;
    l->nodes[i*4 + 2].val = g.datapoints[i].Z;
    l->nodes[i*4 + 3].val = g.datapoints[i].RX;
  }

  // parse mortens input instead
  // double input[40] = {-117.45/2000,-595/2000,-1948.9/2000,4175.3/10000,484.9/2000,-655.3/2000,-2037.05/2000,621.25/10000,373.5/2000,-579.25/2000,-2065.95/2000,174.3/10000,21.95/2000,-529.4/2000,-2023.6/2000,729.2/10000,-63.6/2000,-716.6/2000,-1979.35/2000,76/10000,-191.5/2000,-708.9/2000,-1990.45/2000,590.55/10000,-91.45/2000,-859.9/2000,-1899.55/2000,-1227.95/10000,71.45/2000,-422.95/2000,-2084.75/2000,-2760.85/10000,121.7/2000,-418.3/2000,-2103.3/2000,247.75/10000,-9.7/2000,-496.1/2000,-1958/2000,-702.25/10000};
  // l = ann.layers;
  // for(int i = 0; i < 40; i++) {
  //   l->nodes[i].val = input[i];
  // }


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

  // loop through layers
  //  // Serial.println("--X--,--Y--,--Z--,--RX--");
  // Serial.println();
  // Serial.println();
  // Serial.println();
  // for(l = ann.layers; l < ann.layers + ann.n_layers; l++) {
  //   for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
  //     Serial.print(current->val, 5);Serial.println();
  //   }
  //   Serial.println();
  // }

  // copy final layer node values to result vector
  l = ann.layers+ann.n_layers-1;
  for(int i = 0; i < l->n_nodes; i++) {
    ann.lastResult.results[i] = l->nodes[i].val;
  }

  return ann.lastResult;
}