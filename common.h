//This file contains all the common variables and 
//constants used in the project

#pragma once

//for sensor.h*****************************************
#define GPIO_PIN_IN1 26 //for barcode
#define GPIO_PIN_LEFT_SENSOR 27 //for left sensor
#define GPIO_PIN_RIGHT_SENSOR 28 //for right sensor

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