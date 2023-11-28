/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
// #include "stdio.h"

#include "wheel_encoder.h"
#include "common.h"

int l_triggered = 0;
int r_triggered = 0;

absolute_time_t r_start_time;
absolute_time_t r_prev_time;

absolute_time_t l_start_time;
absolute_time_t l_prev_time;

//define buffers
MessageBufferHandle_t *xMsgBuffer_LeftInterrupt_ptr;
MessageBufferHandle_t *xMsgBuffer_RightInterrupt_ptr;

// stagger number of wheel encoding runs
const int encodingLimit = 10;


/*
    @brief: computes the distance travelled and returns the speed value of a specific wheel
    @param: start_time: The time when an edge rise is detected
            prev_time: The previous time when an edge rise is detected
            dir_triggered: The number of times an edge rise for a wheel is triggered
    @return: speed: Success
*/
float get_dst(absolute_time_t start_time, absolute_time_t prev_time, float dir_triggered){
    // Get the time between the current time and the prev time to get the time elapsed per edge rise
    //
    float elapsed_time = absolute_time_diff_us(prev_time, start_time);
    // Convert to seconds
    //
    float time_secs = elapsed_time / 1000000;
    printf("time secs : %f \n", time_secs);
    // Get speed
    //
    float speed = DISTANCE_STATE / time_secs;
    // Get the distance Travelled
    //
    float distance_travelled = DISTANCE_STATE * dir_triggered;
    printf("Time Elapsed: %.2fs\n", time_secs);
    printf("Distance Travelled: %.2fcm\n", distance_travelled);

    return speed;
}
/*
    @brief: ISR which detects which wheel encoder was triggered and updates and sends speed variable to main
    @params: gpio: pin which was triggered
             events: event which was triggered when the pin was triggered
    @return: None
*/
void wheel_callback(unsigned int gpio, long unsigned int events)
{
    // Left Wheel Encoder
    //
    if (gpio == L_WHEEL_ENCODER)
    {
        l_triggered += 1;
        // Once a previous timing exists
        //
        if (!is_nil_time(l_prev_time) && l_triggered % encodingLimit == 0)        
        {
            l_start_time = get_absolute_time();
            float l_speed = get_dst(l_start_time, l_prev_time, l_triggered);
            printf("Left Wheel Speed: %.2f/s\n", l_speed);
            // Send speed to main
            xMessageBufferSendFromISR(
                /* The message buffer to write to. */
                *xMsgBuffer_LeftInterrupt_ptr,
                /* The source of the data to send. */
                (void *) &l_speed,
                /* The length of the data to send. */
                sizeof( l_speed ),
                /* The block time; 0 = no block */
                NULL 
                );
        
        }
        l_prev_time = get_absolute_time();
    }
    // Right Wheel Encoder
    //
    else if (gpio == R_WHEEL_ENCODER)
    {
        r_triggered += 1;
        // Once a previous timing exists
        //
        if (!is_nil_time(r_prev_time) && r_triggered % encodingLimit == 0) 
        {
            r_start_time = get_absolute_time();
            float r_speed = get_dst(r_start_time, r_prev_time, r_triggered);
            printf("Right Wheel Speed: %.2f/s\n", r_speed);
            //Send speed to main
            xMessageBufferSendFromISR(
                /* The message buffer to write to. */
                *xMsgBuffer_RightInterrupt_ptr,
                /* The source of the data to send. */
                (void *) &r_speed,
                /* The length of the data to send. */
                sizeof( r_speed ),
                /* The block time; 0 = no block */
                NULL 
                );
        }
        r_prev_time = get_absolute_time();
    }
}
/*
    @brief:  Initialize all required pins and pointers to message buffers
    @params: *xMsgBuffer_LeftInterrupt: pointer to the message buffer for the left wheel
             *xMsgBuffer_RightInterrupt: pointer to the message buffer for the right wheel
    @return: None
*/
void initWheelEncoder(MessageBufferHandle_t *xMsgBuffer_LeftInterrupt, MessageBufferHandle_t *xMsgBuffer_RightInterrupt)
{
    xMsgBuffer_LeftInterrupt_ptr = xMsgBuffer_LeftInterrupt;
    xMsgBuffer_RightInterrupt_ptr = xMsgBuffer_RightInterrupt;
    // Initialize all gpio pins
    //
    gpio_init(L_WHEEL_ENCODER);
    gpio_init(R_WHEEL_ENCODER);
    gpio_set_dir(L_WHEEL_ENCODER, GPIO_OUT);
    gpio_set_dir(R_WHEEL_ENCODER, GPIO_OUT);
}
