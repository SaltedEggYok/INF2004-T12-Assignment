/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "main_lib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/pwm.h"
#include "math.h"
#include "time.h"

#include "util/irsensor/sensor.h"
#include "util/map/map.h"
#include "util/wifi/wifi.h"
#include "util/magnetometer/magnetometer.h"
#include "util/ultrasonic/ultrasonic.h"
#include "util/motor_controller/motor_controller.h"
#include "util/wheel_encoder/wheel_encoder.h"
#include "util/pid_controller/pid_controller.h"

/*
    @brief: the callback function when a timer is created
    @param: *t: pointer to the repeating_timer struct
    @return: None
*/
bool repeating_timer_callback(struct repeating_timer *t)
{
    printf("Repeat at %lld\n", time_us_64() / 1000000);
    return true;
}
/*
    @brief:  the main ISR handler which detects which pin is triggered and calls the respective callback function
    @param: gpio: gpio pin which was triggered
            events: event for the gpio which was triggered
    @return: None
*/

void main_callback(unsigned int gpio, long unsigned int events)
{
    // Left Wheel Encoder
    //
    if (gpio == L_WHEEL_ENCODER)
    {
        wheel_callback(gpio, events);
    }
    // Right Wheel Encoder
    //
    else if (gpio == R_WHEEL_ENCODER)
    {
        wheel_callback(gpio, events);
    }
    // UltraSonic Sensor
    //
    else if (gpio == ULTRASONIC_ECHO_PIN)
    {
        ultrasonicCallback(gpio, events);
    }
    else if (gpio == LEFT_IR_SENSOR)
    {
        absolute_time_t start_time;
        // if (leftSensor == 1 && rightSensor == 1)
        if (rightSensor == 1)
        {
            stop_motors(leftSliceNum, rightSliceNum);
            //            add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1 second
            }
            reset_speed(leftSliceNum, rightSliceNum);
            // reverse a bit
            reverse();
            // vTaskDelay(pdMS_TO_TICKS(1500));
            //            add_repeating_timer_ms(-1500, repeating_timer_callback, NULL, &timer);

            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1500000)
            {
                // timer of 1.5 seconds
            }
            // turn left
            turn_left(leftSliceNum, rightSliceNum, true);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1 seconds
            }
            reset_speed(leftSliceNum, rightSliceNum);
            move_forward();
        }
        // else if (leftSensor == 1 && rightSensor == 0)
        else if (rightSensor == 0)
        {

            stop_motors(leftSliceNum, rightSliceNum);
            //            add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);
            start_time = get_absolute_time();

            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1 second
            }
            reset_speed(leftSliceNum, rightSliceNum);
            // reverse a bit
            reverse();
            // vTaskDelay(pdMS_TO_TICKS(1500));
            //            add_repeating_timer_ms(-1500, repeating_timer_callback, NULL, &timer);

            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1500000)
            {
                // timer of 1.5 seconds
            }
            turn_right(leftSliceNum, rightSliceNum, true);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1.5 seconds
            }
            reset_speed(leftSliceNum, rightSliceNum);
            move_forward();
        }
        else
        {
            // should never enter here
            //  go forward
            // move_forward();
        }
    }
    else if (gpio == RIGHT_IR_SENSOR)
    {

        absolute_time_t start_time;

        // if (leftSensor == 1 && rightSensor == 1)
        if (leftSensor == 1)
        {
            stop_motors(leftSliceNum, rightSliceNum);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1 second
            }
            reset_speed(leftSliceNum, rightSliceNum);
            // reverse a bit
            reverse();
            // vTaskDelay(pdMS_TO_TICKS(1500));
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1500000)
            {
                // timer of 1.5 seconds
            }
            // turn left
            turn_left(leftSliceNum, rightSliceNum, true);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1.5 seconds
            }
            reset_speed(leftSliceNum, rightSliceNum);
            move_forward();
        }
        // else if (rightSensor == 1 && leftSensor == 0)
        else if (leftSensor == 0)
        {
            stop_motors(leftSliceNum, rightSliceNum);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1 second
            }
            reset_speed(leftSliceNum, rightSliceNum);
            // reverse a bit
            reverse();
            // vTaskDelay(pdMS_TO_TICKS(1500));
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1500000)
            {
                // timer of 1.5 seconds
            }
            // turn left
            turn_left(leftSliceNum, rightSliceNum, true);
            start_time = get_absolute_time();
            while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000)
            {
                // timer of 1.5 seconds
            }
            reset_speed(leftSliceNum, rightSliceNum);
            move_forward();
        }
        else
        {
            // should never enter here
            //  go forward
            // move_forward();
        }
    }
}

/*
    @brief:  inititalize all required sensors and 
             hardware components along with all pointers which will be used for retrieval
    @param: None
    @return: None
*/
// init everything
void initAll()
{
    // init local variables
    currMode = DEFAULT;
    currMoveState = STATIONARY;
    leftSensor = rightSensor = false;
    xMsgBuffer_LeftInterrupt = xMessageBufferCreate(sizeof(float) * 2);
    xMsgBuffer_RightInterrupt = xMessageBufferCreate(sizeof(float) * 2);

    initSensor(&leftSensor, &rightSensor, &barcodeSensor);
    // initMagnetometer(&magnetometerTimeoutReceived, &compassBearing);
    //  initMap();
    initMotorController(&leftSliceNum, &rightSliceNum, &direction);
    initUltrasonic(&ultrasonicTimeoutReceived, &ultrasonicDistance);
    initWheelEncoder(&xMsgBuffer_LeftInterrupt, &xMsgBuffer_RightInterrupt);
    // initWifi(&wifiEnabled);
}
// const uint BTN_PIN = 14;

void traverseMap()
{
    // traverse map
    // while map is fully seen/ completed
    // do DFS to traverse map, prioritise going left (read current magnetometer)
}

/*
    @brief: defines the default behavior of the car
    @param: None
    @return: None
*/
void defaultBehaviour()
{
    // default behaviour
    // while true, keep going forward
    // if irsensors detect obstacle infront, turn left
    getLeftSensor(&leftSensor);
    getRightSensor(&rightSensor);
    // printf("left: %d, right: %d\n", leftSensor, rightSensor);

    if (leftSensor && rightSensor)
    {
        // turn left
        currMoveState = TURN_LEFT;
        currMode = PERFORMING_TASK;
        // turn_left(&leftSliceNum, &rightSliceNum, &direction);
    }
    else if (leftSensor)
    {
        // turn right
        currMoveState = TURN_RIGHT;
        currMode = PERFORMING_TASK;
        // turn_right(&leftSliceNum, &rightSliceNum, &direction);
    }
    else if (rightSensor)
    {
        // turn left
        currMoveState = TURN_LEFT;
        currMode = PERFORMING_TASK;
        // turn_left(&leftSliceNum, &rightSliceNum, &direction);
    }
    else
    {
        // go forward
        currMoveState = FORWARD;
        currMode = PERFORMING_TASK;
        // move_forward();
    }
}


/*
    @brief: stores all logic that will affect the behavior of the car and updates it
    @param: None
    @return: None
*/
void updateBehaviour()
{
    switch (currMode)
    {
    case DEFAULT:
        defaultBehaviour();
        break;
    case TRAVERSE_MAP:
        traverseMap();
        break;
    case PERFORMING_TASK:
        defaultBehaviour(); // DEBUG LINE
        break;
    default:
        break;
    }
}

/*
    @brief: movement update, purely to call the move functions and update any variables that are affected by movement
    @param: None
    @return: None
*/
void updateMovement()
{
    switch (currMoveState)
    {
    case STATIONARY:
        break;
    case FORWARD:
        move_forward();
        break;
    case BACKWARD:
        reverse();
        break;
    case TURN_LEFT:
        turn_left(leftSliceNum, rightSliceNum, direction);
        break;
    case TURN_RIGHT:
        turn_right(leftSliceNum, rightSliceNum, direction);
        break;
    default:
        break;
    }
}
/*
    @brief: Creates the main task
    @param: None
    @return: None
*/
void mainTask(__unused void *params)
{
    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return;
    }

    // cyw43_arch_enable_sta_mode();

    sleep_ms(5000);
    printf("Starting Main Task \n");

    move_forward();

    float fps = 1;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;

    while (true)
    {
        vTaskDelay(frame_time);
    }
}

/*
    @brief: Creates the detection of obstacle task
    @param: None
    @return: None
*/
void obstacleTask(__unused void *params)
{

    printf("Starting Obstacle Task \n");

    float fps = 3;
    float frame_time = 1000 / fps;

    while (true)
    {
        // If detected object,reverse
        //
        if (ultrasonicDistance <= 10.0)
        {
            // if currently moving forward, reverse
            if (currOrientation == true)
            {
                currOrientation = false;
                reverse();
                vTaskDelay(2 * frame_time);

                // sleep a bit to reverse
            }
        }
        else
        {
            // if currently moving backward, move forward
            if (currOrientation == false)
            {
                currOrientation = true;
                move_forward();
            }
        }
        vTaskDelay(frame_time);
    }
}
/*
    @brief: Initialize the ultrasonic pins and all pointers required
    @param: *ultrasonicTimeoutReceived: pointer to the flag which checks if there is a timeout
            *distanceCM: pointer to the distance variable
    @return: None
*/
void pidTask(__unused void *params)
{

    printf("Starting PID task \n");
    float fps = 20;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;

    float l_speed = 0.0, r_speed = 0.0, fReceivedData;
    size_t xReceivedBytes;
    while (true)
    {
        xReceivedBytes = xMessageBufferReceive(
            xMsgBuffer_LeftInterrupt, /* The message buffer to receive from. */
            (void *)&fReceivedData,   /* Location to store received data. */
            sizeof(fReceivedData),    /* Maximum number of bytes to receive. */
            portMAX_DELAY);           /* Wait indefinitely */

        l_speed = fReceivedData;

        xReceivedBytes = xMessageBufferReceive(
            xMsgBuffer_RightInterrupt, /* The message buffer to receive from. */
            (void *)&fReceivedData,    /* Location to store received data. */
            sizeof(fReceivedData),     /* Maximum number of bytes to receive. */
            portMAX_DELAY);            /* Wait indefinitely */

        r_speed = fReceivedData;

        updated_duty_cycle = compute_pid(r_speed, l_speed, &integral, &prev_error);
        // updated_duty_cycle = compute_pid(l_speed *dt, r_speed *dt, &integral, &prev_error);
        // when intergral becomes negative, means left wheel too fast, reduce,
        // however it is reducing too fast? and then the wheel stops so
        // need to find a way to reduce the speed slowly?
        // but if the wheel ever stops, then speed doesnt update since callback not called
        // so need to find a way to update speed even if callback not called?
        // also when the wheel is turning backwards speed isnt directly negative?
        printf("Modifier: %f\n", updated_duty_cycle);
        duty_cycle += updated_duty_cycle / CLK_CYCLE_NO; // update duty cycle reduce magnitude?
        duty_cycle = MAX(duty_cycle, 0.3f);              // MINUMUM DUTY CYCLE
        duty_cycle = MIN(duty_cycle, 0.8f);              // MAXIMUM DUTY CYCLE
        printf("Modified Duty Cycle : %f\n", duty_cycle);
        update_speed(leftSliceNum, PWM_CHAN_A, duty_cycle);

        // delay frame time
        vTaskDelay(frame_time);
    }
}

/*
    @brief: handles the creation adn starting of all tasks
    @param: None
    @return: None
*/
void vLaunch(void)
{

    TaskHandle_t main_task;
    xTaskCreate(mainTask, "MainThread", configMINIMAL_STACK_SIZE, NULL, 1, &main_task);

    TaskHandle_t sensor_task;
    xTaskCreate(sensorTask, "SensorThread", configMINIMAL_STACK_SIZE, NULL, 5, &sensor_task);

    TaskHandle_t ultrasonic_task;
    xTaskCreate(ultrasonicTask, "UltrasonicThread", configMINIMAL_STACK_SIZE, NULL, 6, &ultrasonic_task);

    TaskHandle_t magnetometer_task;
    // xTaskCreate(magnetometerTask, "MagnetometerThread", configMINIMAL_STACK_SIZE, NULL, 7, &magnetometer_task);

    TaskHandle_t barcode_task;
    // xTaskCreate(barcodeTask, "BarcodeThread", configMINIMAL_STACK_SIZE, NULL, 8, &barcode_task);

    TaskHandle_t obstacle_task;
    xTaskCreate(obstacleTask, "ObstacleThread", configMINIMAL_STACK_SIZE, NULL, 9, &obstacle_task);

    TaskHandle_t pid_task;
    // xTaskCreate(pidTask, "PIDThread", configMINIMAL_STACK_SIZE, NULL, 15, &pid_task);

    // /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

/*
    @brief: The main loop of the program
    @param: None
    @return: 0 : success
*/
int main()
{
    stdio_init_all();

    // sleep to delay starting
    sleep_ms(5000);

    printf("Starting Main \n");
    initAll();

    printf("Init\n");

    // gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &main_callback);
    gpio_set_irq_enabled_with_callback(LEFT_IR_SENSOR, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_IR_SENSOR, GPIO_IRQ_EDGE_RISE, true, &main_callback);

    printf("Callback set\n");

    vLaunch();
}