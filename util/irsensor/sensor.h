#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define GPIO_PIN_IN1 26


//functions 
int startScanningBarcode();
bool isBarcodeComplete();
void scanning();
bool oneCharRead();
void decodeThickThinBar();
const char *returnChar();
void resetForNewString();

void initSensor();