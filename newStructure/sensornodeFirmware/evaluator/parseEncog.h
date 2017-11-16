#ifndef __PARSE_ENCOG__
#define __PARSE_ENCOG__

#include <FS.h>
#include <stdlib.h>
#include "datastructures.h"

// example of usage:
// network ann = parseEncogModel("Test-120-Model-128-encog.txt");

#define MODEL_FILE_PATH "/model"

bool _readInt(File* fp, int* out);
bool _readDouble(File* fp, double* out);
void printWeights(network* ann);
int _parseEncogModel_seekString(File* fp, char* searchString);
void _parseEncogModel_parseNumberOfLayers(File* fp, network* n);
void _parseEncogModel_parseLayerSizes(File* fp, network* n);
void _parseEncogModel_allocateWeightArrays(network* ann);
void _parseEncogModel_parseWeights(File* fp, network* ann);
network parseEncogModel();
void parseEncogModel_setup();

#endif