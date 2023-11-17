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

