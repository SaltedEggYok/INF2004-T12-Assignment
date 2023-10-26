/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "stdio.h"

#define CLK_CYCLE_NO 62500

#define LEFT_MOTOR 20
#define L_REVERSE_PIN 10
#define L_CLOCKWISE_PIN 11

#define RIGHT_MOTOR 21
#define R_REVERSE_PIN 3
#define R_CLOCKWISE_PIN 2


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
// Update pwm parameters to set the speed of a specified motor by modifying the duty cycle
//
void update_speed(uint *slice_num,enum pwm_chan channel, float duty_cycle)
{
    pwm_set_chan_level(*slice_num,channel,CLK_CYCLE_NO * duty_cycle);
}

// Retrieves the slice number attached to a specific motor
//
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

// Enables the car to move forward
//
void move_forward()
{
    gpio_put(L_CLOCKWISE_PIN,1);
    gpio_put(L_REVERSE_PIN,0);
    gpio_put(R_CLOCKWISE_PIN,1);
    gpio_put(R_REVERSE_PIN,0);
}

// Enables the car to reverse
//
void reverse()
{   
    gpio_put(L_CLOCKWISE_PIN,0);
    gpio_put(L_REVERSE_PIN,1);
    gpio_put(R_CLOCKWISE_PIN,0);
    gpio_put(R_REVERSE_PIN,1);
}

// Enables the car to make a left turn
//
void turn_left(uint *l_slice_num,uint *r_slice_num)
{
    update_speed(l_slice_num,PWM_CHAN_A,0.3);
    update_speed(r_slice_num,PWM_CHAN_B,0.8);
    // Checks the current state of the left wheel, sets to the opposite to counteract right wheel movement
    //
    char clockwise_state = gpio_get(L_CLOCKWISE_PIN);
    char reverse_state = gpio_get(L_REVERSE_PIN);
    gpio_put(L_CLOCKWISE_PIN, !clockwise_state);
    gpio_put(L_REVERSE_PIN, !reverse_state);
}

// Enables the car to make a right turn
//
void turn_right(uint *l_slice_num,uint *r_slice_num)
{
    update_speed(l_slice_num,PWM_CHAN_A,0.8);
    update_speed(r_slice_num,PWM_CHAN_B,0.3);
    // Checks the current state of the right wheel, sets to the opposite to counteract left wheel movement
    //
    char clockwise_state = gpio_get(R_CLOCKWISE_PIN);
    char reverse_state = gpio_get(R_REVERSE_PIN);

    gpio_put(R_CLOCKWISE_PIN, !clockwise_state);
    gpio_put(R_REVERSE_PIN, !reverse_state);
}
// Resets the speed back to default
//
void reset_speed(uint *l_slice_num,uint *r_slice_num)
{
    update_speed(l_slice_num,PWM_CHAN_A,0.5);
    update_speed(r_slice_num,PWM_CHAN_B,0.5);    
}

// void clockwise_motor(char motor_direction)
// {   if(motor_direction == 'L')
//     {
//         gpio_set_pulls(L_CLOCKWISE_PIN,true,false);
//         gpio_set_pulls(L_REVERSE_PIN,false,true);
//         enable_motor(LEFT_MOTOR);
//     }
//     else
//     {
//         gpio_set_pulls(R_CLOCKWISE_PIN,true,false);
//         gpio_set_pulls(R_REVERSE_PIN,false,true);
//         enable_motor(RIGHT_MOTOR);
//     }
// }
// void reverse_motor(char motor_direction)
// {
//     if(motor_direction == 'L')
//     {
//         gpio_set_pulls(L_CLOCKWISE_PIN,false,true);
//         gpio_set_pulls(L_REVERSE_PIN,true,false);
//         enable_motor(LEFT_MOTOR);
//     }
//     else
//     {
//         gpio_set_pulls(R_CLOCKWISE_PIN,false,true);
//         gpio_set_pulls(R_REVERSE_PIN,true,false);
//         enable_motor(RIGHT_MOTOR);       
//     }
// }

// Enables the car to stop
//
void stop_motors(uint *l_slice_num,uint *r_slice_num)
{
    // uint l_slice_num = pwm_gpio_to_slice_num(LEFT_MOTOR);
    // uint r_slice_num = pwm_gpio_to_slice_num(RIGHT_MOTOR);

    // if(motor_dir == 'L')
    // {
    //     channel = PWM_CHAN_A;   
    // }
    // else
    // {
    //     channel = PWM_CHAN_B;
    // }
    // printf("Channel: %s\n",channel);
    pwm_set_chan_level(*l_slice_num,PWM_CHAN_A,0);
    pwm_set_chan_level(*r_slice_num,PWM_CHAN_B,0);
    
}

int main() 
{  
    // Initialize all gpio pins
    //
    initialize_gpio_pins();
    // Initalize stdio
    //
    stdio_init_all();
    // Get the respective pwm slices
    //
    uint l_slice_num = get_slice_num(LEFT_MOTOR);
    uint r_slice_num = get_slice_num(RIGHT_MOTOR);

    // Start up the motors
    //
    enable_motors(&l_slice_num,&r_slice_num);
    // move_forward();
    // sleep_ms(15000);
    // stop_motors(&l_slice_num,&r_slice_num);
    // sleep_ms(5000);
    // turn_left(&l_slice_num,&r_slice_num);
    // sleep_ms(5000);
    // turn_right(&l_slice_num,&r_slice_num);
    // sleep_ms(5000);
    // reverse();
    // sleep_ms(5000);
    // stop_motors(&l_slice_num,&r_slice_num);
    while(1)
    {
        char movement = getchar();
        switch(movement)
        {
            case 'f':
            {
                move_forward();
                break;
            }
            case 'l':
            {
                turn_left(&l_slice_num,&r_slice_num);
                break;
            }
            case 't':
            {
                turn_right(&l_slice_num,&r_slice_num);
                break;
            }
            case 's':
            {
                stop_motors(&l_slice_num,&r_slice_num);
                break;
            }
            case 'r':
                reverse();
                break;
            default:
                stop_motors(&l_slice_num,&r_slice_num);
        }
    }
}