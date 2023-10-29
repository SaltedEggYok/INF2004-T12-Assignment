/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "stdio.h"

// To get Wheel Circumference 2piR = 3.25*pi*2 = 20.420
//
#define WHEEL_CIRCUM 20.4
#define DISTANCE_STATE 20.4/20.0
#define R_WHEEL_ENCODER 13


// Motor Controller code to be remvoed after driver end
// 
#define CLK_CYCLE_NO 62500  

#define LEFT_MOTOR 20
#define L_REVERSE_PIN 10
#define L_CLOCKWISE_PIN 11

#define RIGHT_MOTOR 21
#define R_REVERSE_PIN 2
#define R_CLOCKWISE_PIN 3

volatile int l_triggered = 0;
volatile int r_triggered = 0;

uint64_t start_time = 0;
uint64_t prev_time = 0;

void initialize_gpio_pins()
{
    gpio_init(L_REVERSE_PIN);
    gpio_init(L_CLOCKWISE_PIN);
    gpio_init(R_REVERSE_PIN);
    gpio_init(R_CLOCKWISE_PIN);
    gpio_set_dir(L_REVERSE_PIN, GPIO_OUT);
    gpio_set_dir(L_CLOCKWISE_PIN, GPIO_OUT);
    gpio_set_dir(R_REVERSE_PIN, GPIO_OUT);
    gpio_set_dir(R_CLOCKWISE_PIN, GPIO_OUT);
}

uint get_slice_num(int motor_no)
{
    uint slice_num = pwm_gpio_to_slice_num(motor_no);

    return slice_num;
}


// Starts up the motors with default pwm parameters
//
void enable_motors(uint *l_slice_num,uint *r_slice_num)
{
    // Allocate the gpio pins of the left and right motor to the pwm
    //
    gpio_set_function(LEFT_MOTOR,GPIO_FUNC_PWM);   
    gpio_set_function(RIGHT_MOTOR,GPIO_FUNC_PWM);

    // Set the divider of the pwm clock
    //
    pwm_set_clkdiv(*l_slice_num,100);
    pwm_set_clkdiv(*r_slice_num,100);

    // Set the highest value for the pwm before the clock wraps around back to 0
    //
    pwm_set_wrap(*l_slice_num, CLK_CYCLE_NO);
    pwm_set_wrap(*r_slice_num, CLK_CYCLE_NO);

    // Set the default duty cycles of both PWM channels to be 50%
    //
    pwm_set_chan_level(*l_slice_num,PWM_CHAN_A,CLK_CYCLE_NO * 0.5);
    pwm_set_chan_level(*r_slice_num,PWM_CHAN_B,CLK_CYCLE_NO * 0.5);

    // Set the respective pwms to run
    // 
    pwm_set_enabled(*l_slice_num,true);
    pwm_set_enabled(*r_slice_num,true);
}

//
void move_forward()
{
    gpio_put(L_CLOCKWISE_PIN,1);
    gpio_put(L_REVERSE_PIN,0);
    gpio_put(R_CLOCKWISE_PIN,1);
    gpio_put(R_REVERSE_PIN,0);
}



float get_wheel_triggered(int num_triggered)
{
    return num_triggered;
}

float get_dst(float start_time, float prev_time)
{
    float elapsed_time =  start_time - prev_time;
    float time_secs = elapsed_time / 1000000;
    float speed = DISTANCE_STATE/time_secs;
    printf("Time Elapsed: %.2f\n",time_secs);
    printf("Distance Travelled: %.2fcm\n", DISTANCE_STATE * r_triggered);
    printf("Current Speed :%.2f/s\n",speed);

    return speed;
}
void wheel_callback(uint gpio, uint32_t events) 
{

    r_triggered +=1;
    // Once a previous timing exists
    //
    if(prev_time)
    {
        start_time = time_us_64();
        get_dst(start_time,prev_time);
    }
    prev_time = time_us_64();
}

void stop_motors(uint *l_slice_num,uint *r_slice_num)
{
    pwm_set_chan_level(*l_slice_num,PWM_CHAN_A,0);
    pwm_set_chan_level(*r_slice_num,PWM_CHAN_B,0);
}

void update_speed(uint *slice_num,enum pwm_chan channel, float duty_cycle)
{
    pwm_set_chan_level(*slice_num,channel,CLK_CYCLE_NO * duty_cycle);
}

void enable_wheel_encode()
{
    gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE , true, &wheel_callback);
// gpio_set_irq_enabled_with_callback(L_ENCODER, GPIO_IRQ_EDGE_RISE, true, &count_triggered);
}
/*GPIO_IRQ_LEVEL_HIGH GPIO_IRQ_LEVEL_LOW*/
//
int main() {
    // Initialize all gpio pins
    //
    initialize_gpio_pins();
    // Initalize stdio
    //
    stdio_init_all();
    // For driver code showcase purposes
    //
    enable_wheel_encode();
    uint l_slice_num = get_slice_num(LEFT_MOTOR);
    uint r_slice_num = get_slice_num(RIGHT_MOTOR);
    // Start up the motors
    //
    enable_motors(&l_slice_num,&r_slice_num);
    move_forward();
    while(1)
    {
        update_speed(&r_slice_num,PWM_CHAN_A,0.9);
        sleep_ms(5000);
        update_speed(&r_slice_num,PWM_CHAN_A,0.2);
        sleep_ms(5000);
        update_speed(&r_slice_num,PWM_CHAN_A,0.5);
    }
}

