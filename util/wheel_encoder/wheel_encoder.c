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

uint64_t start_time = 0;
uint64_t prev_time = 0;

uint8_t l_slice_num_wheel, r_slice_num_wheel;

void initialize_gpio_pins_wheel()
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

uint8_t get_slice_num_wheel(int motor_no)
{
    uint8_t slice_num = pwm_gpio_to_slice_num(motor_no);

    return slice_num;
}


// Starts up the motors with default pwm parameters
//
void enable_motors_wheel(uint8_t *l_slice_num_wheel,uint8_t *r_slice_num_wheel)
{
    // Allocate the gpio pins of the left and right motor to the pwm
    //
    gpio_set_function(LEFT_MOTOR,GPIO_FUNC_PWM);   
    gpio_set_function(RIGHT_MOTOR,GPIO_FUNC_PWM);

    // Set the divider of the pwm clock
    //
    pwm_set_clkdiv(*l_slice_num_wheel,100);
    pwm_set_clkdiv(*r_slice_num_wheel,100);

    // Set the highest value for the pwm before the clock wraps around back to 0
    //
    pwm_set_wrap(*l_slice_num_wheel, CLK_CYCLE_NO);
    pwm_set_wrap(*r_slice_num_wheel, CLK_CYCLE_NO);

    // Set the default duty cycles of both PWM channels to be 50%
    //
    pwm_set_chan_level(*l_slice_num_wheel,PWM_CHAN_A,CLK_CYCLE_NO * 0.5);
    pwm_set_chan_level(*r_slice_num_wheel,PWM_CHAN_B,CLK_CYCLE_NO * 0.5);

    // Set the respective pwms to run
    // 
    pwm_set_enabled(*l_slice_num_wheel,true);
    pwm_set_enabled(*r_slice_num_wheel,true);
}

//
void move_forward_wheel()
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
    printf("Time Elapsed: %.2fs\n",time_secs);
    printf("Distance Travelled: %.2fcm\n", DISTANCE_STATE * r_triggered);
    printf("Current Speed :%.2f/s\n",speed);

    return speed;
}
void wheel_callback(unsigned int gpio, long unsigned int events) 
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

void stop_motors_wheel(uint8_t *l_slice_num_wheel,uint8_t *r_slice_num_wheel)
{
    pwm_set_chan_level(*l_slice_num_wheel,PWM_CHAN_A,0);
    pwm_set_chan_level(*r_slice_num_wheel,PWM_CHAN_B,0);
}

void update_speed_wheel(uint8_t *slice_num,enum pwm_chan channel, float duty_cycle)
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

void initWheelEncoder(){
    // Initialize all gpio pins
    //
    initialize_gpio_pins_wheel();

    // Start up the motors
    //
    l_slice_num_wheel = get_slice_num_wheel(LEFT_MOTOR);
    r_slice_num_wheel = get_slice_num_wheel(RIGHT_MOTOR);
    enable_motors_wheel(&l_slice_num_wheel,&r_slice_num_wheel);

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

