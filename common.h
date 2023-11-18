//This file contains all the common variables and 
//constants used in the project
#ifndef COMMON_H   
#define COMMON_H
#include <stdint.h>
#include <stdbool.h>

//for ultrasonic.h*****************************************
#define ULTRASONIC_ECHO_PIN 0
#define ULTRASONIC_TRIG_PIN 1

//for sensor.h*****************************************
#define BARCODE_SENSOR 26 //for barcode
#define LEFT_IR_SENSOR 12 //for left sensor
#define RIGHT_IR_SENSOR 13 //for right sensor

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

#define LEFT_MOTOR 7
#define L_REVERSE_PIN 4
#define L_CLOCKWISE_PIN 5

#define RIGHT_MOTOR 6
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

// To get Wheel Circumference 2piR = 3.25*pi*2 = 20.420
//
#define WHEEL_CIRCUM 20.4
#define DISTANCE_STATE 20.4/20.0

#define L_WHEEL_ENCODER 18
#define R_WHEEL_ENCODER 19

// ---------------------------------------------------------------

//for magnetometer.h*****************************************
#define I2C_PORT i2c1

#define ACC_ADDRESS 0x19
#define MAG_ADDRESS 0x1E

// #define MAG_SENSITIVITY 0.67

#define CTRL_REG1_A 0x20

#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D

#define CRA_REG_M 0x00
#define MR_REG_M 0x02
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Z_H_M 0x05
#define OUT_Z_L_M 0x06
#define OUT_Y_H_M 0x07
#define OUT_Y_L_M 0x08

#define PIN_SDA 14
#define PIN_SCL 15

#endif //COMMON_H

// static void printMessage(const char* message);
// static void vTemperatureTask(void* pvParameters);
// static void vMovingAverageTask(void* pvParameters);
// static void vSimpleAverageTask(void* pvParameters);
// static void vPrintTask(void* pvParameters);
/*
Used Pins
2 - right motor reverse
3 - right motor clockwise
4 - left motor reverse
5 - left motor clockwise
6 - right motor
7 - left motor
12 - left sensor
13 - right sensor
14 - SDA, I2C1
15 - SCL, I2C1
18 - left wheel encoder
19 - right wheel encoder
26 - barcode sensor UNUSED
*/

// Configuration Parameters
#define MESSAGE_BUFFER_SIZE 64
#define MAX_MESSAGE_LENGTH 54


extern int16_t bias_x;
extern int16_t bias_y;
extern int16_t bias_z;

extern volatile bool magnetometerTimeoutReceived;
