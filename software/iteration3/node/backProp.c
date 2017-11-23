#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define InputN 40
#define datanum 10
#define OutN 2

#include <backProp.h>

void main(){

}

//sigmoid backprop
void sigmoidBackprop(layer inputLayer, layer outputLayer, double* error){
  int i;
  for (i = 0; i < outputlength; i++){
    inputLayer->nodes[i]->error = outputLayer->nodes->val * (1 - outputLayer->nodes[i]->val) * error[i];
  }
}
//takes three layers as input: an input, output and the netx layer after that,
//which is used to calculate the number of weights for each note in the output layer.
void linearOutputValues(layer inputLayer, layer Outputlayer, layer nextlayer){
  //assuming that the wights have been created
  int i, j;
  double value = 0;
  for(i = 0; i < outputLayer->n_nodes; i++){
    for(j = 0; j < nextLayer->n_nodes){
      value += outputlayer->nodes[i]->weights[j] * inputLayer->nodes[i]->val;
    }
    outputLayer->nodes[i]->val = value;
  }
}

void linearBackprop(layer inputLayer, layer outputLayer, layer nextLayer, double* error, double learningrate){
  int i, j;

  for(i = 0; i < inputLayer->n_nodes; i++){
    for(j = 0; j < nextLayer->n_nodes; j++){
      inputLayer->nodes[i]->weights[j] += learningrate * inputLayer->nodes[i]->val * error[j]
    }
  }
  i = 0;
  j = 0;
  double value = 0.0;
  for(i = 0; i < inputLayer->n_nodes; i++){
    for(j = 0; j < nextLayer->n_nodes; j++){
        value += inputLayer->nodes[i]->weights[j] * error[i];
    }
    inputLayer->nodes[i]->error = value;
  }
}

double* sumSqErrorLayer(double* ys, int ysLength, double* predicted){
  int i;
  double* error;
  for(i = 0; i < ysLength; i++){
    ys[i] - predicted[i];
  }
  return error;
}

double sigmoidOutput(double x) {
  //sigmoid function for a single value
  double returnValue;
  double expValue;
  expValue = exp((double) -x);
  returnValue = 1.0l / (1.0l + expValue);

  return returnValue;
}
