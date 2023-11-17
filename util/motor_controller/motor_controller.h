#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"

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

// forward declaration of enum
enum pwm_chan;

void reset_speed(uint8_t *l_slice_num, uint8_t *r_slice_num);
void move_forward();
void turn_left(uint8_t *l_slice_num, uint8_t *r_slice_num, bool direction);
void turn_right(uint8_t *l_slice_num, uint8_t *r_slice_num, bool direction);
void stop_motors(uint8_t *l_slice_num, uint8_t *r_slice_num);
void update_speed(uint8_t *slice_num, enum pwm_chan channel, float duty_cycle);
void reverse();

void initMotorController(uint8_t *l_slice_num, uint8_t *r_slice_num, bool *direction);

// // safety
// #ifndef CLK_CYCLE_NO
// #define CLK_CYCLE_NO 62500
// #endif

// #ifndef LEFT_MOTOR
// #define LEFT_MOTOR 21
// #endif

// #ifndef L_REVERSE_PIN
// #define L_REVERSE_PIN 2
// #endif

// #ifndef L_CLOCKWISE_PIN
// #define L_CLOCKWISE_PIN 3
// #endif

// #ifndef RIGHT_MOTOR
// #define RIGHT_MOTOR 20
// #endif

// #ifndef R_REVERSE_PIN
// #define R_REVERSE_PIN 10
// #endif

// #ifndef R_CLOCKWISE_PIN
// #define R_CLOCKWISE_PIN 11
// #endif