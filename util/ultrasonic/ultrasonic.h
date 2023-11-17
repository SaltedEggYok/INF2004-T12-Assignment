#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"


#define ULTRASONIC_TRIG_PIN 0
#define ULTRASONIC_ECHO_PIN 1
float getCm();

void initUltrasonic();