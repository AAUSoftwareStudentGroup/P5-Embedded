#ifndef __PARSE_ENCOG__
#define __PARSE_ENCOG__

#include <FS.h>
#include <stdlib.h>
#include "datastructures.h"
#include "config.h"

bool _readInt(File* fp, int* out);
bool _readDouble(File* fp, double* out);
bool _readString(File* fp, char* out);
int _parseEncogModel_seekString(File* fp, char* searchString);
void _parseEncogModel_parseNumberOfLayers(File* fp, network* n);
void _parseEncogModel_parseLayerSizes(File* fp, network* n);
void _parseEncogModel_parsesLabels(File* fp, network* n);
void _parseEncogModel_allocateWeightArrays(network* ann);
void _parseEncogModel_parseWeights(File* fp, network* ann);

network parseEncogModel();
void parseEncogModel_setup();
void printWeights(network* ann);

#endif