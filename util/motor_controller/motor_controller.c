/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "stdio.h"

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

// Resets the speed back to default
//
void reset_speed(uint *l_slice_num,uint *r_slice_num)
{
    update_speed(l_slice_num,PWM_CHAN_A,0.5);
    update_speed(r_slice_num,PWM_CHAN_B,0.5);    
}
// Enables the car to stop
//
void stop_motors(uint *l_slice_num,uint *r_slice_num)
{
    pwm_set_chan_level(*l_slice_num,PWM_CHAN_A,0);
    pwm_set_chan_level(*r_slice_num,PWM_CHAN_B,0);
}

// Enables the car to make a left turn
//
void turn_left(uint *l_slice_num,uint *r_slice_num,uint reverse_turn)
{
    update_speed(l_slice_num,PWM_CHAN_A,0.3);
    update_speed(r_slice_num,PWM_CHAN_B,0.8);
    if(!reverse_turn)
    {
        gpio_put(L_CLOCKWISE_PIN, 0);
        gpio_put(L_REVERSE_PIN, 1);
        gpio_put(R_CLOCKWISE_PIN, 1);
        gpio_put(R_REVERSE_PIN, 0);
    }
    else
    {
        gpio_put(L_CLOCKWISE_PIN, 1);
        gpio_put(L_REVERSE_PIN, 0);
        gpio_put(R_CLOCKWISE_PIN, 0);
        gpio_put(R_REVERSE_PIN, 1);
    }
}

// Enables the car to make a right turn
//
void turn_right(uint *l_slice_num,uint *r_slice_num,uint reverse_turn)
{
    update_speed(l_slice_num,PWM_CHAN_A,0.8);
    update_speed(r_slice_num,PWM_CHAN_B,0.3);
    if(!reverse_turn)
    {
        gpio_put(L_CLOCKWISE_PIN, 1);
        gpio_put(L_REVERSE_PIN, 0);
        gpio_put(R_CLOCKWISE_PIN, 0);
        gpio_put(R_REVERSE_PIN, 1);
    }
    else
    {
        gpio_put(L_CLOCKWISE_PIN, 0);
        gpio_put(L_REVERSE_PIN,1);
        gpio_put(R_CLOCKWISE_PIN, 1);
        gpio_put(R_REVERSE_PIN, 0);
    }
    // Checks if a reverse turn is specified, if not specified, forward turn right else, reverse turn right 
    //
    // if(!reverse_turn)
    // {
    //     gpio_put(L_CLOCKWISE_PIN, 1);n
    //     gpio_put(L_REVERSE_PIN, 0);
    //     gpio_put(R_CLOCKWISE_PIN, 0);
    //     gpio_put(R_REVERSE_PIN, 1);
    // }
    // else
    // {
    //     gpio_put(L_CLOCKWISE_PIN, 1);
    //     gpio_put(L_REVERSE_PIN, 0);
    //     gpio_put(R_CLOCKWISE_PIN, 0);
    //     gpio_put(R_REVERSE_PIN, 1);
    // }
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
    uint reverse_turn = 0;
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
                reset_speed(&l_slice_num,&r_slice_num);
                move_forward();
                break;
            }
            case 'l':
            {
                reset_speed(&l_slice_num,&r_slice_num);
                reverse_turn = 0;
                turn_left(&l_slice_num,&r_slice_num,reverse_turn);
                break;
            }
            case 't':
            {
                reverse_turn = 0;
                turn_right(&l_slice_num,&r_slice_num,reverse_turn);
                break;
            }
            case 's':
            {
                stop_motors(&l_slice_num,&r_slice_num);
                break;
            }
            case 'r':
                update_speed(&l_slice_num,PWM_CHAN_A,0.5);
                update_speed(&r_slice_num,PWM_CHAN_B,0.5);
                reverse();
                break;
            case 'e':
                reverse_turn = 1;
                turn_left(&r_slice_num,&l_slice_num,reverse_turn);
                break;
            case 'j':
                reverse_turn = 1;
                turn_right(&r_slice_num,&l_slice_num,reverse_turn);
                break;
            default:
                reset_speed(&l_slice_num,&r_slice_num);
                stop_motors(&l_slice_num,&r_slice_num);
        }
    }
}