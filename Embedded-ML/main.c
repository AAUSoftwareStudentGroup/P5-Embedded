//#include "GetNetworkFromFile.c"
#include "EvaluateNetwork.h"
#include "parseEncog.h"
#include "ShotIdentifier.h"

#include <stdio.h>
#include <stdlib.h>

void GetInputFromFile(char*, double*);

int main() {
	//GetInputFromFile("input.csv", input);
	group* g;
	int g_len;
	ReadGroupedData("antonvsmorten.csv", &g, &g_len);
	// ReadGroupedData("107shots.csv", &g, &g_len);
	// ReadGroupedData("MORTEN-anton-z-steffan-grunberg-z.csv", &g, &g_len);
	network ann = parseEncogModel("Test-120-Model-128-encog.txt");
	
	double anton = 0;
	double morten = 0;

	group* group;
	for(group = g; group < g+g_len; group++) {
		layer* l = EvaluateNetwork(*group, ann);
		anton += l->nodes[0].val;
		morten += l->nodes[1].val;
	}
	anton  /= g_len;
	morten /= g_len;

	printf("Morten: %lf\nAnton: %lf\n", morten, anton);


	return 0;
}

void GetInputFromFile(char* inputFilePath, double* input) {
	FILE* inputFile = fopen(inputFilePath, "r");
	for(int i = 0; fscanf(inputFile, "%lf;", &(input[i]) ) && feof(inputFile) != 0;i ++);
}