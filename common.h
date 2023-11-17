//This file contains all the common variables and 
//constants used in the project
#ifndef COMMON_H   
#define COMMON_H

#include <stdint.h>

//for barcode sensor*****************************************
#define ULTRASONIC_SENSOR_ECHO 1
#define ULTRASONIC_SENSOR_TRIG 0
//for sensor.h*****************************************
#define BARCODE_SENSOR 26 //for barcode
#define LEFT_IR_SENSOR 27 //for left sensor
#define RIGHT_IR_SENSOR 28 //for right sensor
//for motor_controller.h********************************
#define CLK_CYCLE_NO 62500

// Setup 
// EN A - 21
// EN B - 20
// N1 - 11
// N2 - 10
// N3 - 2
// N4 - 3
// MOTOR WIRE RED NEARER THE 3 PINS SIDE (VBUS SIDE)
// when car facing forward , right side - right 2 
// #define LEFT_MOTOR 21
// #define L_REVERSE_PIN 2
// #define L_CLOCKWISE_PIN 3

// #define RIGHT_MOTOR 20
// #define R_REVERSE_PIN 10
// #define R_CLOCKWISE_PIN 11

#define LEFT_MOTOR 20
#define L_REVERSE_PIN 10
#define L_CLOCKWISE_PIN 11

#define RIGHT_MOTOR 21
#define R_REVERSE_PIN 2
#define R_CLOCKWISE_PIN 3

// For Wheel Encoder --------------------------------------------
//
// volatile int l_triggered = 0;
// volatile int r_triggered = 0;

// uint64_t r_start_time = 0;
// uint64_t r_prev_time = 0;

// uint64_t l_start_time = 0;
// uint64_t l_prev_time = 0;

// static float l_speed = 0.0;
// static float r_speed = 0.0;

// ---------------------------------------------------------------
#endif //COMMON_H

/*
Used Pins
2 - left motor reverse
3 - left motor clockwise
10 - right motor reverse
11 - right motor clockwise
12 - left wheel encoder
13 - right wheel encoder
20 - right motor
21 - left motor
26 - barcode sensor UNUSED
27 - left sensor
28 - right sensor
*/