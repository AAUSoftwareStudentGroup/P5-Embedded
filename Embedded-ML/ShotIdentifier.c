#include "ShotIdentifier.h"

void ReadGroupedData(char* filePath, group** shots, int* shots_len){
	FILE *ifp = fopen(filePath, "r");

    int* dataArray = malloc(sizeof(int)*7*3552000);

    // read first line, as it includes headers
    fscanf(ifp, "%*[^\n]\n");

    for(int i = 0; i < 3552000; i++){
        fscanf(ifp, "%d;%d;%d;%d;%d;%d;%d",
            &dataArray[7*i],&dataArray[(7*i)+1],&dataArray[(7*i)+2],&dataArray[(7*i)+3],&dataArray[(7*i)+4],&dataArray[(7*i)+5],&dataArray[(7*i)+6] );
    }

    downscaleAndGroupData(dataArray, 3552000, shots, shots_len);
}

void downscaleAndGroupData(int* data, int n_data, group** shots, int* shots_len) {
    datapoint* average;
    int average_len;

    _downscale(data, n_data, &average, &average_len);
    _groupData(average, average_len, shots, shots_len);

    printf("%d samples, %d shots\n", average_len, *shots_len );
}

void _downscale(int* data, int n_data, datapoint** average, int* average_len) {
    int downscaleFactor = 20;


    *average = malloc(sizeof(datapoint)*(n_data/downscaleFactor));
    int i = 0;
    int n_average = 0;

    for(i = 0; i < (n_data/downscaleFactor); i++) {
        (*average)[i] = (datapoint){0, 0, 0, 0, 0, 0, 0};
    }

    while((n_average + 1) * downscaleFactor <= n_data) {
        for(int j = n_average*downscaleFactor; j < (n_average+1)*downscaleFactor; j++) {
            (*average)[n_average].T += data[j*7+_T];
            (*average)[n_average].X += data[j*7+_X];
            (*average)[n_average].Y += data[j*7+_Y];
            (*average)[n_average].Z += data[j*7+_Z];
            (*average)[n_average].RX += data[j*7+_RX];
            (*average)[n_average].RY += data[j*7+_RY];
            (*average)[n_average].RZ += data[j*7+_RZ];
        }
        //printf("RX: %lf \n", (*average)[n_average].RX);
        n_average++;
    }

    for(int i = 0; i < (n_data/downscaleFactor); i++) {
        (*average)[i].T /= downscaleFactor;
        (*average)[i].X /= downscaleFactor;
        (*average)[i].Y /= downscaleFactor;
        (*average)[i].Z /= downscaleFactor;
        (*average)[i].RX /= downscaleFactor;
        (*average)[i].RY /= downscaleFactor;
        (*average)[i].RZ /= downscaleFactor;
        //printf("RX-avg: %lf \n", (*average)[i].RX);
    }
    *average_len =  n_average;
}

void _groupData(datapoint* data, int n_data, group** groups, int* groups_len) {
    int zeroStreak = 0;
    _Bool measuring = 0;
    int k;
    *groups_len = 0;
    group g;
    g.length = 0;
    g.datapoints = NULL;

    *groups = malloc(sizeof(group)*(n_data/10));

    for(k = 0; k < n_data; k++){
        if(measuring){
            g.length++;
        }
        if(data[k].RX < 5000 && data[k].RX > -5000){
            if(measuring == 1) {
                zeroStreak++;
                if(zeroStreak == 10){
                    (*groups)[*groups_len] = g;
                    (*groups_len)++;
                    
                    measuring = 0;
                }
            }
        } else {
            if(measuring == 0) {
                g.length = 0;
                g.datapoints = &data[k];
                measuring = 1;
            }
            zeroStreak = 0;
        }
    }
}
