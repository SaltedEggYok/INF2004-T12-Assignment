/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor_controller.h"


/*
    @brief: Initializes all the gpio pins needed for the left and right wheel
    @param: None
    @return: None
*/
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


/*
    @brief: Update pwm parameters to set the speed of a specified motor by modifying the duty cycle
    @param: slice_num: slice_number of the left or right wheel
            channel: PWM channel to be updated
            duty_cycle: value to multiply to obtain the duty cycle
    @return: None
*/
void update_speed(uint8_t slice_num, enum pwm_chan channel, float duty_cycle)
{
    printf("Updating speed\n");
    pwm_set_chan_level(slice_num, channel, CLK_CYCLE_NO * duty_cycle);
    printf("Updated speed\n");
}

/*
    @brief: Retrieves the slice number attached to a specific motor
    @param: motor_no: GPIO pin number of motor wanted
    @return: slice_num: Success
*/
uint8_t get_slice_num(int motor_no)
{
    uint8_t slice_num = pwm_gpio_to_slice_num(motor_no);

    return slice_num;
}

/*
    @brief: enables the motors to start running at default speed
    @param: l_slice_num: slice number of left motor
            r_slice_num: slice number of right motor
    @return: None
*/
void enable_motors(uint8_t l_slice_num, uint8_t r_slice_num)
{
    // Allocate the gpio pins of the left and right motor to the pwm
    //
    gpio_set_function(LEFT_MOTOR, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_MOTOR, GPIO_FUNC_PWM);

    // Set the divider of the pwm clock
    //
    pwm_set_clkdiv(l_slice_num, 100);
    pwm_set_clkdiv(r_slice_num, 100);

    // Set the highest value for the pwm before the clock wraps around back to 0
    //
    pwm_set_wrap(l_slice_num, CLK_CYCLE_NO);
    pwm_set_wrap(r_slice_num, CLK_CYCLE_NO);

    // Set the default duty cycles of both PWM channels to be 50%
    //
    pwm_set_chan_level(l_slice_num, PWM_CHAN_A, CLK_CYCLE_NO * 0.8);
    pwm_set_chan_level(r_slice_num, PWM_CHAN_B, CLK_CYCLE_NO * 0.8);

    // Set the respective pwms to run
    //
    pwm_set_enabled(l_slice_num, true);
    pwm_set_enabled(r_slice_num, true);
}

/*
    @brief: enables the motors to move forward
    @param: None
    @return: None
*/
void move_forward()
{
    gpio_put(L_CLOCKWISE_PIN, 1);
    gpio_put(L_REVERSE_PIN, 0);
    gpio_put(R_CLOCKWISE_PIN, 1);
    gpio_put(R_REVERSE_PIN, 0);
}
/*
    @brief: enables the motors to reverse
    @param: None
    @return: None
*/
void reverse()
{
    printf("REVERSING\n");
    gpio_put(L_CLOCKWISE_PIN, 0);
    gpio_put(L_REVERSE_PIN, 1);
    gpio_put(R_CLOCKWISE_PIN, 0);
    gpio_put(R_REVERSE_PIN, 1);
}

/*
    @brief: resets the speed of the motor to default speed
    @param: l_slice_num: slice number of left motor
            r_slice_num: slice number of right motor
    @return: None
*/
void reset_speed(uint8_t l_slice_num, uint8_t r_slice_num)
{
    update_speed(l_slice_num, PWM_CHAN_A, 0.5);
    update_speed(r_slice_num, PWM_CHAN_B, 0.5);
}
/*
    @brief: resets the speed of the motor to 0 to stop the motor
    @param: l_slice_num: slice number of left motor
            r_slice_num: slice number of right motor
    @return: None
*/
void stop_motors(uint8_t l_slice_num, uint8_t r_slice_num)
{
    pwm_set_chan_level(l_slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(r_slice_num, PWM_CHAN_B, 0);
}

/*
    @brief: allows the motor to turn left
    @param: l_slice_num: slice number of left motor
            r_slice_num: slice number of right motor
            direction: boolean flag to check if motor does a reverse turn or a forward turn
    @return: None
*/
void turn_left(uint8_t l_slice_num, uint8_t r_slice_num, bool direction)
{
    // If direction flag is specified, do a normal left turn, else do a reverse left turn
    if (direction)
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

/*
    @brief: allows the motor to turn right
    @param: l_slice_num: slice number of left motor
            r_slice_num: slice number of right motor
            direction: boolean flag to check if motor does a reverse turn or a forward turn
    @return: None
*/
void turn_right(uint8_t l_slice_num, uint8_t r_slice_num, bool direction)
{
     // If direction flag is specified, do a normal right turn, else do a reverse right turn
    if (direction)
    {
        gpio_put(L_CLOCKWISE_PIN, 1);
        gpio_put(L_REVERSE_PIN, 0);
        gpio_put(R_CLOCKWISE_PIN, 0);
        gpio_put(R_REVERSE_PIN, 1);
    }
    else
    {
        gpio_put(L_CLOCKWISE_PIN, 0);
        gpio_put(L_REVERSE_PIN, 1);
        gpio_put(R_CLOCKWISE_PIN, 1);
        gpio_put(R_REVERSE_PIN, 0);
    }
}
/*
    @brief: Initializes the motor controller pins and retrieves the slice numbers
    @param: *l_slice_num: pointer to the slice number of left motor
            *r_slice_num: pointer to the slice number of right motor
            *direction: pointer to the direction boolean flag
    @return: None
*/
void initMotorController(uint8_t *l_slice_num, uint8_t *r_slice_num, bool *direction)
{
    // Initialize all gpio pins
    //
    initialize_gpio_pins();
    // Get the respective pwm slices
    //
    *l_slice_num = get_slice_num(LEFT_MOTOR);
    *r_slice_num = get_slice_num(RIGHT_MOTOR);
    *direction = true;

    enable_motors(*l_slice_num, *r_slice_num);
}
