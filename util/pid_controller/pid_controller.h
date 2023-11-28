#pragma once
#include <math.h>
// #include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "common.h"

float compute_pid(float target_spd, float curr_spd, float *integral, float *prev_error);