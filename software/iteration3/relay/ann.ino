#include "ann.h"

inline float sigmoid(float x) {
  return 1.0l / (1.0l + exp((float) -x) );
}

inline float derivedSigmoid(float x) {
  float a = sigmoid(x);
  return a*(1-a);
}

networkResult EvaluateNetwork(network* ann, group g) {
  layer* l;
  node* current; 
  node* next;
  int nextNodeIndex;
  
  // init node values to zero
  for(l = ann->layers; l < ann->layers + ann->n_layers; l++) {
    for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
      current->val = 0;
      current->out = 0;
    }
  }

  l = ann->layers;
  for(int i = 0; i < 10; i++) {
    l->nodes[i*4 + 0].out = g.datapoints[i].X;
    l->nodes[i*4 + 1].out = g.datapoints[i].Y;
    l->nodes[i*4 + 2].out = g.datapoints[i].Z;
    l->nodes[i*4 + 3].out = g.datapoints[i].RX;
  }

  // loop through layers
  for(l = ann->layers; l < ann->layers + ann->n_layers-1; l++) {
    for(current = l->nodes; current < l->nodes+l->n_nodes; current++) {
      for(nextNodeIndex = 0, next = (l+1)->nodes; nextNodeIndex < (l+1)->n_nodes; nextNodeIndex++, next++) {
        next->val += current->out*current->weights[nextNodeIndex];
      }
    }
    for(nextNodeIndex = 0, next = (l+1)->nodes; nextNodeIndex < (l+1)->n_nodes; nextNodeIndex++, next++) {
      next->val += l->bias.weights[nextNodeIndex];
      next->out = sigmoid(next->val);
    }
  }

  // copy final layer node values to result vector
  l = ann->layers+ann->n_layers-1;
  for(int i = 0; i < l->n_nodes; i++) {
    ann->lastResult.results[i] = l->nodes[i].out;
  }

  return ann->lastResult;
}

network initiateRandomNetwork(char** labels) {
  const int n_layers = 3;
  int n_labels = NUMBER_OF_LABELS;
  int layerSizes[n_layers] = {INPUT_NEURONS, HIDDEN_NEURONS, NUMBER_OF_LABELS};

  network n;

  //allocate layers, n_layers, lastresult, labels
  n.n_layers = n_layers;
  n.layers = (layer*)malloc(sizeof(layer)*n_layers);

  int n_outputNodes = layerSizes[n_layers - 1];
  n.lastResult.length = n_outputNodes;
  n.lastResult.results = (float*)malloc(sizeof(float)*n_outputNodes);

  n.labels = (char**)malloc(sizeof(char*)*n_outputNodes);
  for(int i = 0; i < n_outputNodes; i++) {
    n.labels[i] = (char*)malloc(sizeof(char)*8);
    if(i < n_labels)
      strcpy(n.labels[i], labels[i]);
    else
      strcpy(n.labels[i], ("Label" + String(i)).c_str());
  }

  // for each layer
  for(int i = 0; i < n_layers; i++) {
    // allocate space for nodes
    n.layers[i].n_nodes = layerSizes[i];
    n.layers[i].nodes = (node*)malloc(sizeof(node)*layerSizes[i]);

    // if not the last layer
    if(i != n_layers-1) {
      // for each node in layer
      for(int j = 0; j < layerSizes[i]; j++) {
        // Allocate space for weights
        n.layers[i].nodes[j].weights = (float*)malloc(sizeof(float)*layerSizes[i+1]);

        // for each of those weights
        for(int k = 0; k < layerSizes[i+1]; k++) {
          // allocate a random value
          float r = ((float) random(1000)/1000.0l) - 0.5l;
          n.layers[i].nodes[j].weights[k] = r;
        }
      }
      // Allocate space for weights on bias node
      n.layers[i].bias.weights = (float*)malloc(sizeof(float)*layerSizes[i+1]);
      n.layers[i].bias.out = 1;
      // for each of those weights
      for(int k = 0; k < layerSizes[i+1]; k++) {
        // allocate a random value
        float r = ((float) random(1000)/1000.0l) - 0.5l;
        n.layers[i].bias.weights[k] = r;
      }
    }
  }
  return n;
}

void deallocateNetwork(network* n) {
  // free layers
  for(int i = 0; i < n->n_layers; i++) {
    // free the layers nodes
    for(int j = 0; j < n->layers[i].n_nodes; j++) {
      free(n->layers[i].nodes[j].weights);
    } 
    free(n->layers[i].nodes);

    // free the layers bias nodes weights
    free(n->layers[i].bias.weights);
  }
  free(n->layers);

  // free labels
  for(int i = 0; i < n->lastResult.length; i++) {
    free(n->labels[i]);
  }
  free(n->labels);

  // free lastResult
  free(n->lastResult.results);
}


bool _calculateOutputError(network* n, networkResult expectedOutput) {
  float error; // error of a single output node
  layer* outputLayer = n->layers+(n->n_layers-1);
  float sumSquaredError = 0;
  
  // if length of expected and actual output does not match
  if(outputLayer->n_nodes != expectedOutput.length) {
    return false;
  }
  for(int i = 0; i < outputLayer->n_nodes; i++) {
    // error = (expected-result)*derivedSigmoid(val)
    error = (expectedOutput.results[i] - outputLayer->nodes[i].out);
    sumSquaredError += error*error;
    error *= derivedSigmoid(outputLayer->nodes[i].val);
    outputLayer->nodes[i].error = error;
  }

  return true; 
}

void _backpropogateErrorValues(network* n){
  // calculate the index of the second last layer.
  int layerOut = n->n_layers - 2;
  // iterate over all layers except the first and last
  for(int i = layerOut; i >= 1; i--){
    // iterate over all the nodes in the current layer
    for(int j = 0; j < n->layers[i].n_nodes; j++){

      // calculate error term for the node
      float errorSum = 0; // sum of (errors times weights)
      for(int k = 0; k < n->layers[i + 1].n_nodes; k++){
        errorSum += n->layers[i + 1].nodes[k].error * n->layers[i].nodes[j].weights[k];
      }
      n->layers[i].nodes[j].error = errorSum * derivedSigmoid(n->layers[i].nodes[j].val);
    }
  }
}

void _updateWeights(network* n, float learningRate) {
  // for all layers except last
  for(int i = 0; i < n->n_layers-1; i++) {
    // for each node in that layer
    for(int j = 0; j < n->layers[i].n_nodes; j++) {
      // for each outgoing weight 
      // (number of outgoing weights is the same as the number of nodes in the next layer)
      for(int k = 0; k < n->layers[i+1].n_nodes; k++) {
        float newWeight;
        float oldWeight          = n->layers[i].nodes[j].weights[k];
        float weightTargetError  = n->layers[i+1].nodes[k].error;
        float weightOriginOutput = n->layers[i].nodes[j].out;
        
        newWeight = oldWeight + (learningRate*weightTargetError*weightOriginOutput);
        n->layers[i].nodes[j].weights[k] = newWeight;
      }
    }
    // also handle the bias nodes weights
    {
      // for each outgoing weight
      // (number of outgoing weights is the same as the number of nodes in the next layer)
      for(int k = 0; k < n->layers[i+1].n_nodes; k++) {
        float newWeight;
        float oldWeight          = n->layers[i].bias.weights[k];
        float weightTargetError  = n->layers[i+1].nodes[k].error;
        float weightOriginOutput = n->layers[i].bias.out;
        
        newWeight = oldWeight + (learningRate*weightTargetError*weightOriginOutput);
        n->layers[i].bias.weights[k] = newWeight;
      }
    }
  }
}

networkResult trainNetwork(network* n, example* examples, int n_examples) {
  networkResult res;
  // for each example
  for(int i = 0; i < n_examples; i++) {
    // feed it though the network
    res = EvaluateNetwork(n, examples[i].input);

    // calculate squared errors on the output
    _calculateOutputError(n, examples[i].output);

    // back propogate error
    _backpropogateErrorValues(n);

    // update weights based on errors
    _updateWeights(n, LEARNING_RATE);

    yield();
  }

  return res;
}
