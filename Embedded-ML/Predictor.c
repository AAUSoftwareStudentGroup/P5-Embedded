#include <stdio.h>
#include <stdlib.h>
#include <math.h>
double sigmoid(double neuron);
void prediction(int L, int *LS, double *input);

int main(int argc, const char * argv[]){
    //3 input, 4 hidden neurons, 2 output
    int L = 3;
    int LS[3] = {3, 4, 2};
    double input[3] = {3.1, 3.3, 4.1}; 
    double synapse1[3][4] = {
        {0.1, 0.3, 0.2, 0.5},
        {0.1, 0.3, 0.2, 0.5},
        {0.1, 0.3, 0.2, 0.5}
    };
    
    double synapse2[4][2] = {
        {0.1, 0.5},
        {0.1, 0.3},
        {0.1, 0.2},
        {0.3, 0.1}
    };
    
    double *_input;
    int *_LS;
    
    _input = input;
    _LS = LS;
    
    prediction(L, _LS, _input);
    
    return 0;
}

double sigmoid(double x){
    double returnValue;
    double expValue;
    
    
    
    expValue = exp((double) -x);
    
    returnValue = 1 / (1 + expValue);
    
    return returnValue;
}

void prediction(int L, int *LS, double *input){
    int i, j, k;
    double weight = 0.0;
    double hidden[4] = {0, 0, 0, 0};
    double output[2] = {0, 0};
    
    double synapse1[3][4] = {
        {0.1, 0.3, 0.2, 0.5},
        {0.1, 0.3, 0.2, 0.5},
        {0.1, 0.3, 0.2, 0.5}
    };
    
    
    double synapse2[4][2] = {
        {0.1, 0.5},
        {0.1, 0.3},
        {0.1, 0.2},
        {0.3, 0.1}
    };
    
    for(i = 1; i < L; i++){
        printf("synapse number: %d. it has outputs %d neurons.\n", i, LS[i]);
        for(j = 0; j < LS[i]; j++){
            printf("calculating neuron %d, with %d weights\n", j + 1, LS[i-1]);
            for(k = 0; k < LS[i-1]; k++){
                if(i == 1){
                    weight += (synapse1[k][j] * *(input + k));
                    printf("Adding weight %d (j is %d) : %lf\n", k, j, weight);
                }else{
                     weight += (synapse2[k][j] * hidden[k]);
                     printf("Adding weight %d (j is %d) : %lf\n", k, j, weight);
                }
            } 
            printf("the weight is: %lf\n", weight);
            if(i == 1){
                hidden[j] = sigmoid(weight);
            }else{
                output[j] = sigmoid(weight);
            }
            weight = 0.0;
        }
        if(i == 1){
            printf("\nhidden: %lf, %lf, %lf\n\n", hidden[0], hidden[1], hidden[2]);
        }
    }
            printf("\n output: %lf, %lf\n", output[0], output[1]);
}