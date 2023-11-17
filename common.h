//This file contains all the common variables and 
//constants used in the project

#pragma once



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
#define LEFT_MOTOR 21
#define L_REVERSE_PIN 2
#define L_CLOCKWISE_PIN 3

#define RIGHT_MOTOR 20
#define R_REVERSE_PIN 10
#define R_CLOCKWISE_PIN 11

// For Wheel Encoder --------------------------------------------
//
volatile int l_triggered = 0;
volatile int r_triggered = 0;

uint64_t r_start_time = 0;
uint64_t r_prev_time = 0;

uint64_t l_start_time = 0;
uint64_t l_prev_time = 0;

static float l_speed = 0.0;
static float r_speed = 0.0;
// ---------------------------------------------------------------