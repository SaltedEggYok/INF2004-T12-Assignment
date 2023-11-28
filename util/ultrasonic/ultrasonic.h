#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"

float getCM();
float getPulse();

void initUltrasonic(volatile bool * ultrasonicTimeoutReceived, volatile float *distanceCM);
void triggerUltrasonic();

void ultrasonicTask(__unused void *params);
void ultrasonicCallback(uint gpio, uint32_t events);
