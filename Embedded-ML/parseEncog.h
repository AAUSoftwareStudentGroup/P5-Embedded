#ifndef __PARSE_ENCOG__
#define __PARSE_ENCOG__

#include <stdio.h>
#include <stdlib.h>
#include "datastructures.h"

// example of usage:
// network ann = parseEncogModel("Test-120-Model-128-encog.txt");


int _parseEncogModel_seekString(FILE* fp, char* searchString);
void _parseEncogModel_parseNumberOfLayers(FILE* fp, network* n);
void _parseEncogModel_parseLayerSizes(FILE* fp, network* n);
void _parseEncogModel_allocateWeightArrays(network* ann);
void _parseEncogModel_parseWeights(FILE* fp, network* ann);
network parseEncogModel(char* filePath);

#endif