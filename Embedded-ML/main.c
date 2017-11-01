#include "GetNetworkFromFile.c"
#include "EvaluateNetwork.c"

#include <stdio.h>
#include <stdlib.h>

void GetInputFromFile(char*, double*);

int main() {
	printf("Allocating variables\n");
	double** layerWeights = malloc(sizeof(double*));
	int* layerSizes = malloc(sizeof(int)*10);
	double* output = malloc(sizeof(double));
	int outputSize = 40;
	int layers = -1;
	
	// double input[40];// = malloc(sizeof(double)*40);
	double* input = malloc(sizeof(double)*40);
	double** inputP = (double**)&input;
	
	printf("GetInputFromFile\n");

	for(int i = 0; i < outputSize; i++) {
		input[i] = 1;
	}

	GetInputFromFile("input.csv", input);
	

	printf("GetNetworkFromFile\n");
	GetNetworkFromFile("Test-120-Model-128-encog.txt", layerWeights, layerSizes, &layers);
	
	for(int layer = 0; layer < layers;layer++ ) {
		for(int layerIndex = 0; layerIndex < layerSizes[layer]; layerIndex++) {
			for(int layerIndex2 = 0; layerIndex2 < layerSizes[layer+1]; layerIndex2++) {
				printf("layerWeight %i, %i, %i: %lf\n",layer, layerIndex, layerIndex2, layerWeights[layer][layerIndex*layerSizes[layer]+layerIndex2]);
			}
		}
	}
	output = EvaluateNetwork(layerWeights, layerSizes, layers, input);

	// Print network output
	for(int i = 0; i < outputSize; i++) {
		printf("%.1f ", input[i]);
	}
	printf("\n");

	return 0;
}

void GetInputFromFile(char* inputFilePath, double* input) {
	FILE* inputFile = fopen(inputFilePath, "r");
	int i = 0;
	while( fscanf(inputFile, "%lf;", &(input[i]) ) && feof(inputFile) != 0);
}