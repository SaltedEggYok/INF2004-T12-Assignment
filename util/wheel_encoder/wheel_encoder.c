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

// linked to main_lib.h
// volatile float l_speed = NULL;
// volatile float r_speed = NULL;

//define buffers
MessageBufferHandle_t *xMsgBuffer_LeftInterrupt_ptr;
MessageBufferHandle_t *xMsgBuffer_RightInterrupt_ptr;

//stagger number of wheel encoding runs
const int encodingLimit = 10;



// Get the speed value of either wheel
//
// float get_dst(float start_time, float prev_time, float dir_triggered)
// {
//     // Get the time between the current time and the prev time to get the time elapsed per edge rise
//     //
//     float elapsed_time = start_time - prev_time;
//     // Convert to seconds
//     //
//     float time_secs = elapsed_time / 1000000;
//     printf("time secs : %f \n", time_secs);
//     // Get speed
//     //
//     float speed = DISTANCE_STATE / time_secs;
//     // Get the distance Travelled
//     //
//     float distance_travelled = DISTANCE_STATE * dir_triggered;
//     printf("Time Elapsed: %.2fs\n", time_secs);
//     printf("Distance Travelled: %.2fcm\n", distance_travelled);

//     return speed;
// }
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

void wheelEncoderTask(__unused void *params)
{
    printf("Starting Wheel Encoder Task \n");
    float fps = 1;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;
    while (true)
    {

        printf("Wheel Encoder Task \n");



        // delay frame time
        vTaskDelay(frame_time);
    }
}

// ISR which detects which wheel encoder was triggered and updates speed variable
//
void wheel_callback(unsigned int gpio, long unsigned int events)
{
    // Left Wheel Encoder
    //
    if (gpio == L_WHEEL_ENCODER)
    {
        // // get_dst(l_start_time,l_prev_time,l_triggered);
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
        // printf("L_prev_time: %d\n",l_prev_time);
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

// void wheel_callback(unsigned int gpio, long unsigned int events)
// {
//     if(gpio == R_WHEEL_ENCODER)
//     {
//         r_triggered +=1;
//         // Once a previous timing exists
//         //
//         if(r_prev_time)
//         {
//             r_start_time = time_us_64();
//             r_speed = get_dst(r_start_time,r_prev_time,r_triggered);
//         }
//         r_prev_time = time_us_64();
//     }
//     if(gpio == L_WHEEL_ENCODER)
//     {
//         l_triggered +=1;
//         // Once a previous timing exists
//         //
//         if(l_prev_time)
//         {
//             l_start_time = time_us_64();
//             l_speed = get_dst(l_start_time,l_prev_time,l_triggered);
//         }
//         l_prev_time = time_us_64();
//     }
// }

// Enables the ISR for both wheel encoders
//
// void enable_wheel_encode()
// {
//     gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE , true, &wheel_callback);
//     gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &wheel_callback);
// }

// Initialize wheel encoder
//
void initWheelEncoder(MessageBufferHandle_t *xMsgBuffer_LeftInterrupt, MessageBufferHandle_t *xMsgBuffer_RightInterrupt)
{
    // linking pointers
    // l_speed_ptr = l_speed;
    // r_speed_ptr = r_speed;
    xMsgBuffer_LeftInterrupt_ptr = xMsgBuffer_LeftInterrupt;
    xMsgBuffer_RightInterrupt_ptr = xMsgBuffer_RightInterrupt;

    // Initialize all gpio pins
    //
    gpio_init(L_WHEEL_ENCODER);
    gpio_init(R_WHEEL_ENCODER);
    gpio_set_dir(L_WHEEL_ENCODER, GPIO_OUT);
    gpio_set_dir(R_WHEEL_ENCODER, GPIO_OUT);
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
