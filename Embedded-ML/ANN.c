#include <stdio.h>
#include <stdlib.h>
double sigmoid(double neuron);
void fill_ANN(double ** LW, int *LS);

int main(void){
    //3 input, 4 hidden neurons, 2 output
    int i, j, k, L = 3;
    double weight = 0.0;
    int LS[3] = {3, 4, 2}; 
    
    
    double input[3] = {3.1, 3.3, 4.1};
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
                    weight += (synapse1[k][j] * input[k]);
                    printf("Adding weight %d (j is %d) : %lf\n", k, j, weight);
                }else{
                     weight += (synapse2[k][j] * hidden[k]);
                     printf("Adding weight %d (j is %d) : %lf\n", k, j, weight);
                }
            } 
            printf("the weight is: %lf\n", weight);
            if(i == 0){
                hidden[j] = weight;
            }else{
                output[j] = weight;
            }
            weight = 0.0;
        }
        
        printf("\n output: %lf, %lf\n", output[0], output[1]);
        
    }
    
    /*
    double ** LW;
    int * LS;
    test();
    fill_ANN(&LW, &LS);
    */
    return 0;
}

double sigmoid(double neuron){
    //int *arr = (int*)malloc(size*sizeof(int));
    return 1/(neuron);
}

void fill_ANN(double ** LW, int *LS){
    
    
}