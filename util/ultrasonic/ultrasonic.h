#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"


float getCm(volatile bool *echoReceived, absolute_time_t startTime_ultra, absolute_time_t endTime_ultra,int timeout,bool *ultraSonicTimeoutReceived);
float getPulse(volatile bool *echoReceived, absolute_time_t startTime_ultra, absolute_time_t endTime_ultra,int timeout,bool *ultraSonicTimeoutReceived);

void initUltrasonic();
void triggerUltrasonic();
