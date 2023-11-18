#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//#include "common.h"


//functions 
int startScanningBarcode();
bool isBarcodeComplete();
void scanning();
bool oneCharRead();
void decodeThickThinBar();
const char *returnChar();
void resetForNewString();

void initSensor();

//pass by reference from main, to get sensor readings
//for normal map traversing line reading
//will be called almost every frame
void getLeftSensor(bool* leftSensor);
void getRightSensor(bool* rightSensor);