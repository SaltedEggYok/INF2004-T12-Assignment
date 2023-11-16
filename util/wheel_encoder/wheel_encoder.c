/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
//#include "stdio.h"

#include "wheel_encoder.h"

volatile int l_triggered = 0;
volatile int r_triggered = 0;

uint64_t r_start_time = 0;
uint64_t r_prev_time = 0;

uint64_t l_start_time = 0;
uint64_t l_prev_time = 0;

static float l_speed = 0.0;
static float r_speed = 0.0;

uint8_t l_slice_num_wheel, r_slice_num_wheel;


// Get the speed value of either wheel
//
float get_dst(float start_time, float prev_time,float dir_triggered)
{
    float elapsed_time =  start_time - prev_time;
    float time_secs = elapsed_time / 1000000;
    float speed = DISTANCE_STATE/time_secs;
    printf("Time Elapsed: %.2fs\n",time_secs);
    printf("Distance Travelled: %.2fcm\n", DISTANCE_STATE * dir_triggered);
    printf("Current Speed :%.2f/s\n",speed);

    return speed;
}
// ISR which detects which wheel encoder was triggered and updates speed variable
//
void wheel_callback(unsigned int gpio, long unsigned int events) 
{
    if(gpio == 13)
    {
        r_triggered +=1;
        // Once a previous timing exists
        //
        if(r_prev_time)
        {
            r_start_time = time_us_64();
            r_speed = get_dst(r_start_time,r_prev_time,r_triggered);
        }
        r_prev_time = time_us_64();
    }
    if(gpio == 12)
    {
        l_triggered +=1;
        // Once a previous timing exists
        //
        if(l_prev_time)
        {
            l_start_time = time_us_64();
            l_speed = get_dst(l_start_time,l_prev_time,l_triggered);
        }
        l_prev_time = time_us_64();
    }
}

// Enables the ISR for both wheel encoders
//
void enable_wheel_encode()
{
    gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE , true, &wheel_callback);
    gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &wheel_callback);
}

// Initialize wheel encoder
//
void initWheelEncoder()
{
    // Initialize all gpio pins
    //
    gpio_init(L_WHEEL_ENCODER);
    gpio_init(R_WHEEL_ENCODER);
    gpio_set_dir(L_WHEEL_ENCODER,GPIO_OUT);
    gpio_set_dir(R_WHEEL_ENCODER,GPIO_OUT);
}

// int main() {
//     // Initalize stdio
//     //
//     stdio_init_all();
//     // For driver code showcase purposes
//     //
//     enable_wheel_encode();
//     // // uint l_slice_num_wheel = get_slice_num_wheel(LEFT_MOTOR);
//     // // uint r_slice_num_wheel = get_slice_num_wheel(RIGHT_MOTOR);

//     // enable_motors_wheel(&l_slice_num_wheel,&r_slice_num_wheel);
//     //move_forward_wheel();
//     while(1)
//     {
//         update_speed_wheel(&r_slice_num_wheel,PWM_CHAN_B,0.9);
//         sleep_ms(5000);
//         update_speed_wheel(&r_slice_num_wheel,PWM_CHAN_B,0.2);
//         sleep_ms(5000);
//         update_speed_wheel(&r_slice_num_wheel,PWM_CHAN_B,0.5);
//     }
// }

