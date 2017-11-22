#ifndef __DATASTRUCTURES__
#define __DATASTRUCTURES__

typedef struct _datapoint
{
  double T;
  double X;
  double Y;
  double Z;
  double RX;
  double RY;
  double RZ;
} datapoint;

#define _T 0
#define _X 1
#define _Y 2
#define _Z 3
#define _RX 4
#define _RY 5
#define _RZ 6

#endif
