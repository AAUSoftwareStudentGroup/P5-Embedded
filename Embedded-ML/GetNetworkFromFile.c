#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void GetNetworkFromFile(char* file, double** weights, int* layerSizes, int* layers) {
	FILE* inputFile = fopen(file, "r");
	char* string = malloc(sizeof(char)*40000);

	// LayerSizes
	while( fscanf(inputFile, "layerCounts=%s\n", string ) == 0) fgets(string, 40000, inputFile);
	int i = 0;
	char* token = strtok(string, ",");	
	int* layerSizesTemp = malloc(sizeof(int)*10);
	do {
		sscanf(token, "%i", &layerSizesTemp[i++]);
		token = strtok(NULL, ",");
		//printf("Read: \"%i\"\n", layerSizes[i]);
	} while( token != NULL );

	*layers = i; 
	i--;
	for(int j = i; j >= 0; j--) {
		layerSizes[i-j] = layerSizesTemp[j];
	}
	free(layerSizesTemp);

	// Weights
	*weights = malloc(sizeof(double*)*(*layers));
	for(int j = 1; j < *layers; j++) {
		weights[j-1] = malloc(sizeof(double) * (layerSizes[j] * layerSizes[j-1]));
	}

	while( fscanf(inputFile, "weights=%s\n", string ) == 0) fgets(string, 40000, inputFile);

	token = strtok(string, ",");

	// Put weights into right layers
	i = *layers;
	i--;
	int h = 0;
	int j = layerSizes[i]*layerSizes[i-1];

	for(int layer = 0; layer < layers; layer++) {
		
	}
	do {
		h++;
		sscanf(token, "%lf", &weights[i-1][j--]);
		token = strtok(NULL, ",");
		if (j % layerSizes[i-1] == 0)
			j--;
		if (j < 0) {  // Vi har lige skrevet den sidste vægt i det her lag, hop videre til næste
			i -= 1;
			j = layerSizes[i]*layerSizes[i-1];
		} 
		//printf("Read: \"%lf\"\n", *weights[i]);
	} while( token != NULL );
}

// Weights: weights1-2 weights2-3

// weights1-2: 1[1]-2[1] 1[1]-2[2] ....