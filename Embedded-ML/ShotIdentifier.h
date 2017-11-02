#ifndef __SHOT_IDENTIFIER__
#define __SHOT_IDENTIFIER__ 

#include <stdio.h>
#include <stdlib.h>

#include "datastructures.h"

void ReadGroupedData(char* filePath, group** shots, int* shots_len);
void downscaleAndGroupData(int* data, int n_data, group** shots, int* shots_len);
void _downscale(int* data, int n_data, datapoint** average, int* average_len);
void _groupData(datapoint* data, int n_data, group** groups, int* groups_len);

#endif
