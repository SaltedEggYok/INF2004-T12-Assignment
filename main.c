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

#include "util/irsensor/sensor.h"
#include "util/map/map.h"
#include "util/wifi/wifi.h"
#include "util/magnetometer/magnetometer.h"
#include "util/ultrasonic/ultrasonic.h"
#include "util/motor_controller/motor_controller.h"
#include "util/wheel_encoder/wheel_encoder.h"
#include "util/pid_controller/pid_controller.h"


void main_callback(unsigned int gpio, long unsigned int events)
{
    // printf("Callback\n");
    //printf("Callback GPIO: %d\n", gpio);
    // Left Wheel Encoder
    //
    if (gpio == L_WHEEL_ENCODER)
    {
        wheel_callback(gpio, events);
        // // get_dst(l_start_time,l_prev_time,l_triggered);
        // l_triggered += 1;
        // printf("Left Wheel Encoder Triggered: %d\n", l_triggered);
        // // Once a previous timing exists
        // //
        // if (l_prev_time)
        // {
        //     l_start_time = time_us_64();
        //     l_speed = get_dst(l_start_time, l_prev_time, l_triggered);
        //     printf("Left Wheel Speed: %.2f/s\n", l_speed);
        // }
        // l_prev_time = time_us_64();
    }
    // Right Wheel Encoder
    //
    else if (gpio == R_WHEEL_ENCODER)
    {
        wheel_callback(gpio, events);
        // r_triggered += 1;

        // // Once a previous timing exists
        // //
        // if (r_prev_time)
        // {
        //     r_start_time = time_us_64();
        //     r_speed = get_dst(r_start_time, r_prev_time, r_triggered);
        //     printf("Right Wheel Speed: %.2f/s\n", r_speed);
        // }
        // r_prev_time = time_us_64();
    }
    // UltraSonic Sensor
    //
    else if (gpio == 1)
    {
        // do nothing
    }
    // IR sensor 1
    //
    else if (gpio == 26)
    {
        // do nothing
    }
    else if (gpio == ULTRASONIC_ECHO_PIN)
    {
        ultrasonicCallback(gpio, events);
        // if (gpio_get(ULTRASONIC_ECHO_PIN) == 1)
        // {
        //     startTime_ultra = get_absolute_time();
        // }
        // else
        // {
        //     endTime_ultra = get_absolute_time();
        //     echoReceived = true;
        // }
    }
}

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

// behaviour update, all logic that will affect the
// behaviour of the car will be here
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

        // do nothing?? .. for now??
        break;
    default:
        break;
    }
}

// movement update, purely to call the move functions
// and update any variables that will BE AFFECTED the movement - not AFFECTING
void updateMovement()
{
    switch (currMoveState)
    {
    case STATIONARY:
        break;
    case FORWARD:
        move_forward();
        // sleep_ms(5000);
        // turn_left(&leftSliceNum, &rightSliceNum, direction);
        // sleep_ms(5000);
        // turn_right(&leftSliceNum, &rightSliceNum, direction);
        // sleep_ms(5000);
        // reverse();
        // sleep_ms(5000);
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
        // if(leftSensor != NULL && rightSensor != NULL)
        //{
        //printf("Left Sensor Main: %d, Right Sensor Main: %d\n", leftSensor, rightSensor);
        //}
        vTaskDelay(frame_time);
    }
}

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
            xMsgBuffer_LeftInterrupt,        /* The message buffer to receive from. */
            (void *) &fReceivedData,      /* Location to store received data. */
            sizeof( fReceivedData ),      /* Maximum number of bytes to receive. */
            portMAX_DELAY );              /* Wait indefinitely */

        l_speed = fReceivedData;
        
        xReceivedBytes = xMessageBufferReceive( 
            xMsgBuffer_RightInterrupt,        /* The message buffer to receive from. */
            (void *) &fReceivedData,      /* Location to store received data. */
            sizeof( fReceivedData ),      /* Maximum number of bytes to receive. */
            portMAX_DELAY );              /* Wait indefinitely */

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

        //delay frame time
        vTaskDelay(frame_time);
    }
}


// task launching function
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
    //xTaskCreate(barcodeTask, "BarcodeThread", configMINIMAL_STACK_SIZE, NULL, 8, &barcode_task);

    TaskHandle_t obstacle_task;
    xTaskCreate(obstacleTask, "ObstacleThread", configMINIMAL_STACK_SIZE, NULL, 9, &obstacle_task);

    TaskHandle_t pid_task;
    //xTaskCreate(pidTask, "PIDThread", configMINIMAL_STACK_SIZE, NULL, 15, &pid_task);

    TaskHandle_t wheel_encoder_task;
    //xTaskCreate(wheelEncoderTask, "WheelEncoderThread", configMINIMAL_STACK_SIZE, NULL, 10, &wheel_encoder_task);

    // /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main()
{
    stdio_init_all();

    //sleep to delay starting
    sleep_ms(5000);

    printf("Starting Main \n");
    initAll();

    printf("Init\n");

    // gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &main_callback);

    printf("Callback set\n");

    vLaunch();

    // while(true)
    // {
    //     updated_duty_cycle = compute_pid(r_speed, l_speed, &integral, &prev_error);
    //     // updated_duty_cycle = compute_pid(l_speed *dt, r_speed *dt, &integral, &prev_error);
    //     // when intergral becomes negative, means left wheel too fast, reduce,
    //     // however it is reducing too fast? and then the wheel stops so
    //     // need to find a way to reduce the speed slowly?
    //     // but if the wheel ever stops, then speed doesnt update since callback not called
    //     // so need to find a way to update speed even if callback not called?
    //     // also when the wheel is turning backwards speed isnt directly negative?
    //     printf("Modifier: %f\n", updated_duty_cycle);
    //     duty_cycle += updated_duty_cycle / CLK_CYCLE_NO; // update duty cycle reduce magnitude?
    //     duty_cycle = MAX(duty_cycle, 0.3f);              // MINUMUM DUTY CYCLE
    //     duty_cycle = MIN(duty_cycle, 0.8f);              // MAXIMUM DUTY CYCLE
    //     printf("Modified Duty Cycle : %f\n", duty_cycle);
    //     update_speed(leftSliceNum, PWM_CHAN_A, duty_cycle);
    // }

    // }
    // sleep_ms(5000);
    // printf("Starting...\n");

    // // gpio_set_dir(BTN_PIN, GPIO_IN);
    // // gpio_set_pulls(BTN_PIN, true, false);

    // // if (cyw43_arch_init()) {
    // //     printf("Wi-Fi init failed.");
    // //     return -1;
    // // }
    // // init everything
    // initAll();

    // sleep_ms(2000);
    // printf("Init Passed...\n");

    // xTaskCreate(vTemperatureTask,"Temp_Task",configMINIMAL_STACK_SIZE,NULL,8,NULL);
    // xTaskCreate(vMovingTask,"Moving_Task",configMINIMAL_STACK_SIZE,NULL,7,NULL);
    // gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER, GPIO_IRQ_EDGE_RISE, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(LEFT_IR_SENSOR,GPIO_IRQ_EDGE_RISE,false,&main_callback);
    // gpio_set_irq_enabled_with_callback(RIGHT_IR_SENSOR,GPIO_IRQ_EDGE_RISE,false,&main_callback);
    // gpio_set_irq_enabled_with_callback(BARCODE_SENSOR,GPIO_IRQ_EDGE_RISE,false,&main_callback);

    // vTaskStartScheduler();

    // absolute_time_t frame_start;
    // absolute_time_t frame_end;
    // uint16_t frame_duration;
    // float fps = 10;
    // float frame_time = 1000 / fps;
    // float dt = frame_time / 1000;
    // printf("Frame Time: %f\n", frame_time);

    // //short sleep
    // sleep_ms(100);

    // while (true)
    // {
    //     frame_start = get_absolute_time();

    //     printf("Left Sensor Main : %d\n", leftSensor);
    //     printf("Right Sensor Main : %d\n", rightSensor);

    //     // // Correct Left Wheel Speed to Right Wheel
    //     // //
    //     // printf("L_speed : %f\n",l_speed);
    //     // printf("R_speed: %f\n",r_speed);
    //     // updated_duty_cycle = compute_pid(r_speed *dt, l_speed *dt, &integral, &prev_error);

    //     // //updated_duty_cycle = compute_pid(l_speed *dt, r_speed *dt, &integral, &prev_error);
    //     // //when intergral becomes negative, means left wheel too fast, reduce,
    //     // //however it is reducing too fast? and then the wheel stops so
    //     // //need to find a way to reduce the speed slowly?
    //     // //but if the wheel ever stops, then speed doesnt update since callback not called
    //     // //so need to find a way to update speed even if callback not called?
    //     // //also when the wheel is turning backwards speed isnt directly negative?
    //     // printf("Modifier: %f\n", updated_duty_cycle);
    //     // duty_cycle += updated_duty_cycle / CLK_CYCLE_NO; //update duty cycle reduce magnitude?
    //     // duty_cycle = MAX(duty_cycle, 0.3f); //MINUMUM DUTY CYCLE
    //     // duty_cycle = MIN(duty_cycle, 0.8f); //MAXIMUM DUTY CYCLE
    //     // printf("Modified Duty Cycle : %f\n", duty_cycle);
    //     // update_speed(leftSliceNum, PWM_CHAN_A, duty_cycle);

    //     //update_speed(rightSliceNum, PWM_CHAN_B, duty_cycle);
    //     // printf("curr Move State: %d\n", currMoveState);
    //     // printf("curr Mode: %d\n", currMode);

    //     // updateBehaviour();
    //     // updateMovement();

    //     // double distance_cm = getCm(&echoReceived,startTime_ultra,endTime_ultra,timeout,&ultrasonicTimeoutReceived);

    //     // if (!ultrasonicTimeoutReceived) {
    //     //     printf("Distance from nearest object: %.2f (cm)\n", distance_cm);
    //     // } else {
    //     //     printf("Timeout reached.\n");
    //     //     ultrasonicTimeoutReceived = false;
    //     // }
    //     // int16_t x_acc, y_acc, z_acc;
    //     // int16_t x_mag, y_mag, z_mag;

    //     // bool magTimeout;
    //     // getMagnetometerTimeout(&magTimeout);

    //     // if (!magTimeout)
    //     // {
    //     //     readAccelerometerData(&x_acc, &y_acc, &z_acc);
    //     //     readMagnetometerData(&x_mag, &y_mag, &z_mag);
    //     //     // convertMagDataToUT(&x_mag, &y_mag, &z_mag);
    //     //     calculateAcceleration(x_acc, y_acc, z_acc);
    //     //     // printf("Accelerometer Data: (X = %d, Y = %d, Z = %d)\n", x_acc, y_acc, z_acc);
    //     //     // printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", x_mag, y_mag, z_mag);

    //     //     double compass_bearing = getCompassBearing(x_mag, y_mag);
    //     //     // printf("Compass Bearing: %.2f degrees\n", compass_bearing);
    //     // }
    //     // else
    //     // {
    //     //     // printf("Magnetometer read timed out.\n");
    //     //     // magnetometerTimeoutReceived = false;
    //     //     setMagnetometerTimeout(false);
    //     // }

    //     // if(gpio_get(BTN_PIN))
    //     // {
    //     //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    //     //     sleep_ms(250);
    //     //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    //     //     sleep_ms(250);
    //     // }

    //     // getting fps
    //     frame_end = get_absolute_time();
    //     frame_duration = absolute_time_diff_us(frame_start, frame_end);
    //     printf("Sleep Duration: %f\n", frame_time - frame_duration / 1000); //DEBUG
    //     sleep_ms(frame_time - frame_duration / 1000);

    //     // sleep_ms(1000/60); // 60 fps
    // }
}