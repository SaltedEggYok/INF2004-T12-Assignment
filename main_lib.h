/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <stdio.h>
#include "common.h"

//maybe
enum MODE{
    DEFAULT,
    TRAVERSE_MAP,
    PERFORMING_TASK,
};

enum MOVE_STATE{
    STATIONARY,
    FORWARD,
    TURN_LEFT,
    TURN_RIGHT,
    BACKWARD,
};

enum MODE currMode;
enum MOVE_STATE currMoveState;

//for use with sensor.h
bool leftSensor = false;
bool rightSensor = false;

//for use with motor_controller.h
uint8_t leftSliceNum, rightSliceNum; 
bool direction = false;

//for use with wiif.h
bool wifiEnabled = false;

//for use with wheel_encoder.h
volatile int l_triggered = 0;
volatile int r_triggered = 0;

uint64_t r_start_time = 0;
uint64_t r_prev_time = 0;

uint64_t l_start_time = 0;
uint64_t l_prev_time = 0;

static float l_speed = 0.0;
static float r_speed = 0.0;

static float duty_cycle = 0.8;
static float updated_duty_cycle = 0.0;

float integral = 0.0;
float prev_error = 0.0;


//for use with ultrasonic.h

//const uint trigPin = 0; // GP0
//const uint echoPin = 1; // GP1

volatile absolute_time_t startTime_ultra;
volatile absolute_time_t endTime_ultra;

const int timeout = 26100; // Timeout in microseconds (100ms)
volatile bool echoReceived = false;
bool ultrasonicTimeoutReceived = false;


//for use with magnetometer.h
// int16_t bias_x = 0;
// int16_t bias_y = 0;
// int16_t bias_z = 0;

// volatile bool magnetometerTimeoutReceived = false;

// #define CLK_CYCLE_NO 62500

// // Setup 
// // EN A - 21
// // EN B - 20
// // N1 - 11
// // N2 - 10
// // N3 - 2
// // N4 - 3
// // MOTOR WIRE RED NEARER THE 3 PINS SIDE (VBUS SIDE)
// // when car facing forward , right side - right 2 
// #define LEFT_MOTOR 21
// #define L_REVERSE_PIN 2
// #define L_CLOCKWISE_PIN 3

// #define RIGHT_MOTOR 20
// #define R_REVERSE_PIN 10
// #define R_CLOCKWISE_PIN 11
