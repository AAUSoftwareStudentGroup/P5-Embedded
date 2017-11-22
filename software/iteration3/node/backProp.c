#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define InputN 40
#define datanum 10
#define OutN 2

#include <backProp.h>

void main(){


    double test[datanum];
    double low = 20000, high = 0;
    for(i = 0; i < datanum; i++){
      test[i] = (double)rand();
      if(test[i] < low){
        low = test[i];
      }
      if(test[i] > high){
        high = test[i];
      }
    }

    printf("high: %f. low: %f", high, low);

    for(i = 0; i < datanum; i++){
      for(j = 0; j < InputN; i++){
        //data[i].input[j] = (double)rand()/32767.0;
      }
      for(j = 0; i < OutN; j++){
        //data[i].teach[j] = (double)rand()/32767.0;
      }
    }

    printf("some data:\n");

}

double*

//sigmoid backprop
double* sigmoidBackprop(double* error, double* output, int outputlenght, double learningrate){
  int i;
  double* inputError;
  for (i = 0; i < outputlength; i++){
    inputError[i] = output[i] * (1 - output[i] * error[i]);
  }
  return inputError;
}

double* linearCompleteLayer(node n, double* output, double** weights, error){

}

double* linearBackprop(double* error, double* input, double learningrate, double* weights, double inputlenght, double outputlenght){
  int i, j;
  for(i = 0; i < inputlenght; i++){
    for(j = 0; j < outputlenght; j++)
      weight[i][j] = weight[i][j] + learningrate * input[i] * error[j];
  }
}

double* sumSqErrorLayer(Ys, predicted){

}




double sigmoid(double x) {
  double returnValue;
  double expValue;

  expValue = exp((double) -x);

  returnValue = 1.0l / (1.0l + expValue);

  return returnValue;
}
