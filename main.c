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
#include "message_buffer.h"

#include "util/irsensor/sensor.h"
#include "util/map/map.h"
#include "util/wifi/wifi.h"
#include "util/magnetometer/magnetometer.h"
#include "util/ultrasonic/ultrasonic.h"
#include "util/motor_controller/motor_controller.h"
#include "util/wheel_encoder/wheel_encoder.h"
#include "util/pid_controller/pid_controller.h"

// static void printMessage(const char* message);
static void vSendL(void* pvParameters);
static void vSendR(void* pvParameters);
static void vReceieveL(void* pvParameters);
static void vReceiveR(void* pvParameters);
// static void vMovingAverageTask(void* pvParameters);
// static void vSimpleAverageTask(void* pvParameters);
// static void vPrintTask(void* pvParameters);
static MessageBufferHandle_t xL_WheelBuffer;
static MessageBufferHandle_t xR_WheelBuffer;
// static MessageBufferHandle_t xSimpleAvgBuffer;

void main_callback(unsigned int gpio, long unsigned int events) 
{
    printf("Callback\n");
    printf("GPIO: %d\n",gpio);
    // Left Wheel Encoder
    // 
    if(gpio == L_WHEEL_ENCODER)
    {
        // get_dst(l_start_time,l_prev_time,l_triggered);
        l_triggered +=1;
        // Once previous timing exists
        //
        if(l_prev_time)
        {
            l_start_time = time_us_64();
            l_speed = get_dst(l_start_time,l_prev_time,l_triggered,&l_dist);
            printf("Left Wheel Speed: %.2f/s\n",l_speed);
        }
        l_prev_time = time_us_64();
    }
    // Right Wheel Encoder
    // 
    else if(gpio == R_WHEEL_ENCODER)
    {
        r_triggered +=1;
        // Once a previous timing exists
        //
        if(r_prev_time)
        {
            r_start_time = time_us_64();
            r_speed = get_dst(r_start_time,r_prev_time,r_triggered,&r_dist);
            printf("Right Wheel Speed: %.2f/s\n",r_speed);
        }
        r_prev_time = time_us_64();

    }
    // UltraSonic Sensor
    //
    else if(gpio == 1)
    {
        //do nothing
    }
    // IR sensor 1
    //
    else if(gpio == 26)
    {
        //do nothing 
    }
    else if(gpio == ULTRASONIC_ECHO_PIN)
    {
        if (gpio_get(ULTRASONIC_ECHO_PIN) == 1) 
        {
            startTime_ultra = get_absolute_time();
        } 
        else 
        {
            endTime_ultra = get_absolute_time();
            echoReceived = true;
        }
    }
}

// init everything
void initAll()
{

    // init local variables
    currMode = DEFAULT;
    currMoveState = STATIONARY;
    leftSensor = rightSensor = false;

    initSensor();
    initMagnetometer();
    calibrateAccelerometer();
    //initMap();
    initMotorController(&leftSliceNum, &rightSliceNum, &direction);
    initUltrasonic();
    initWheelEncoder();
    //initWifi(&wifiEnabled);
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
    //printf("left: %d, right: %d\n", leftSensor, rightSensor);

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
        defaultBehaviour(); //DEBUG LINE
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
        turn_left(&leftSliceNum, &rightSliceNum, direction);
        break;
    case TURN_RIGHT:
        turn_right(&leftSliceNum, &rightSliceNum, direction);
        break;
    default:
        break;
    }
}

// Send Message to the Left Wheel Buffer
//
static void vSendL(void* pvParameters)
{
    // Constantly send left wheel speed to the PID controller
    //
    while (1)
    {
        xMessageBufferSendFromISR(xL_WheelBuffer,(void *)&l_speed,sizeof(l_speed),0);
    }
}
// Receive Message from Right Wheel Buffer
// 
static void vReceiveL(void* pvParameters)
{
    // Constantly send left wheel speed to the PID controller
    //
    while (1)
    {
        xMessageBufferReceiveFromISR(xL_WheelBuffer,(void *)&l_speed,sizeof(l_speed),0);
    }
}
// Send Message to Right Wheel Buffer
//
static void vSendR(void* pvParameters)
{
    while (1)
    {
        xMessageBufferSendFromISR(xR_WheelBuffer,(void *)&r_speed,sizeof(r_speed),0);
    }
}
// Receive Message from Right Wheel Buffer
//
static void vReceiveR(void* pvParameters)
{
    // Constantly send left wheel speed to the PID controller
    //
    while (1)
    {
        xMessageBufferReceiveFromISR(xR_WheelBuffer,(void *)&r_speed,sizeof(r_speed),0);
    }
}

int main()
{
    stdio_init_all();

    sleep_ms(5000);
    printf("Starting...\n");
    initAll();
    sleep_ms(2000);
    printf("Init Passed...\n");
    // xTaskCreate(vTemperatureTask,"Temp_Task",configMINIMAL_STACK_SIZE,NULL,8,NULL);
    // xTaskCreate(vMovingTask,"Moving_Task",configMINIMAL_STACK_SIZE,NULL,7,NULL);
    gpio_set_irq_enabled_with_callback(L_WHEEL_ENCODER,GPIO_IRQ_EDGE_RISE,true,&main_callback);
    gpio_set_irq_enabled_with_callback(R_WHEEL_ENCODER,GPIO_IRQ_EDGE_RISE,true,&main_callback);
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &main_callback);
    // gpio_set_irq_enabled_with_callback(LEFT_IR_SENSOR,GPIO_IRQ_EDGE_RISE,false,&main_callback);
    // gpio_set_irq_enabled_with_callback(RIGHT_IR_SENSOR,GPIO_IRQ_EDGE_RISE,false,&main_callback);
    // gpio_set_irq_enabled_with_callback(BARCODE_SENSOR,GPIO_IRQ_EDGE_RISE,false,&main_callback);

    //vTaskStartScheduler();
    move_forward();
    // Create Message Buffer to send 
    //
    xL_WheelBuffer = xMessageBufferCreate(MESSAGE_BUFFER_SIZE);
    xR_WheelBuffer = xMessageBufferCreate(MESSAGE_BUFFER_SIZE);
    xTaskCreate(vSendL, "LeftSpeedTask", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
    xTaskCreate(vSendR, "RightSpeedTask", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
    vTaskStartScheduler();

    while (true) 
    {
        //printf("curr Move State: %d\n", currMoveState);
        //printf("curr Mode: %d\n", currMode);
    
        //updateBehaviour();
        //updateMovement();
        double distance_cm = getCm(&echoReceived,startTime_ultra,endTime_ultra,timeout,&ultrasonicTimeoutReceived);
        // updated_duty_cycle = compute_pid(r_speed,l_speed,&integral,&prev_error);
        // duty_cycle += updated_duty_cycle;
        // printf("Updated Duty Cycle : %f",duty_cycle);
        //update_speed(&leftSliceNum,PWM_CHAN_A,duty_cycle);
        if (!ultrasonicTimeoutReceived) {
            printf("Distance from nearest object: %.2f (cm)\n", distance_cm);
        } else {
            printf("Timeout reached.\n");
            ultrasonicTimeoutReceived = false;
        }
        
        int16_t x_acc, y_acc, z_acc;
        int16_t x_mag, y_mag, z_mag;

        if(!magnetometerTimeoutReceived) 
        {
            readAccelerometerData(&x_acc, &y_acc, &z_acc);
            readMagnetometerData(&x_mag, &y_mag, &z_mag);
            // convertMagDataToUT(&x_mag, &y_mag, &z_mag);
            calculateAcceleration(x_acc, y_acc, z_acc);
            // printf("Accelerometer Data: (X = %d, Y = %d, Z = %d)\n", x_acc, y_acc, z_acc);
            printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", x_mag, y_mag, z_mag);

            double compass_bearing = getCompassBearing(x_mag, y_mag);
            printf("Compass Bearing: %.2f degrees\n", compass_bearing);
        
        }
        else 
        {
            printf("Magnetometer read timed out.\n");
            magnetometerTimeoutReceived = false;
        }


        // if(gpio_get(BTN_PIN))
        // {
        //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        //     sleep_ms(250);
        //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        //     sleep_ms(250);
        // }

        // pseudo code
    }
}