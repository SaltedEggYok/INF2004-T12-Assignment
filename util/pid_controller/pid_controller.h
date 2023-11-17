#pragma once
#include <math.h>
// #include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>


// #include "hardware/pwm.h"

#define CLK_CYCLE_NO 62500
#define LEFT_MOTOR 2
#define RIGHT_MOTOR 3

float car_angle = 40.0;
float initial_angle = 0.0;
// static float duty_cycle = 0.5;
enum pwm_chan;

// uint get_slice_num(int motor_no);
char determine_wheel(float inital_angle, float curr_angle);
float compute_pid(float target_spd, float curr_spd, float *integral, float *prev_error);