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
    //printf("Callback\n");
    printf("Callback GPIO: %d\n",gpio);
    // Left Wheel Encoder
    // 
    if(gpio == L_WHEEL_ENCODER)
    {
        // get_dst(l_start_time,l_prev_time,l_triggered);
        l_triggered +=1;
        // Once a previous timing exists
        //
        if(l_prev_time)
        {
            l_start_time = time_us_64();
            l_speed = get_dst(l_start_time,l_prev_time,l_triggered);
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
            r_speed = get_dst(r_start_time,r_prev_time,r_triggered);
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




// void readMagnetometerData(int16_t* x, int16_t* y, int16_t* z) {
//     *x = (int16_t)((readRegister(MAG_ADDRESS, OUT_X_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_X_L_M));
//     *y = (int16_t)((readRegister(MAG_ADDRESS, OUT_Y_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_Y_L_M));
//     *z = (int16_t)((readRegister(MAG_ADDRESS, OUT_Z_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_Z_L_M));

//     if ((*x == -16192 && *y == -16192 && *z == -16192) || 
//         (*x == 16448 && *y == 16448 && *z == 16448) || 
//         (*x == -32640 && *y == -32640 && *z == -32640) || 
//         (*x == 0 && *y == 0 && *z == 0)) {
//         magnetometerTimeoutReceived = true;
//     }
// }

// void calibrateAccelerometer() {
//     const int numSamples = 100;  
//     int16_t x_accum = 0;
//     int16_t y_accum = 0;
//     int16_t z_accum = 0;

//     for (int i = 0; i < numSamples; i++) {
//         int16_t x, y, z;
//         readAccelerometerData(&x, &y, &z);
//         x_accum += x;
//         y_accum += y;
//         z_accum += z;
//         sleep_ms(10);  
//     }

//     bias_x = x_accum / numSamples;
//     bias_y = y_accum / numSamples;
//     bias_z = z_accum / numSamples;
// }

// void readAccelerometerData(int16_t* x, int16_t* y, int16_t* z) {
//     *x = (int16_t)((readRegister(ACC_ADDRESS, OUT_X_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_X_L_A)) - bias_x;
//     *y = (int16_t)((readRegister(ACC_ADDRESS, OUT_Y_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_Y_L_A)) - bias_y;
//     *z = (int16_t)((readRegister(ACC_ADDRESS, OUT_Z_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_Z_L_A)) - bias_z;
// }

// init everything
void initAll()
{

    // init local variables
    currMode = DEFAULT;
    currMoveState = STATIONARY;
    leftSensor = rightSensor = false;

    initSensor();
    initMagnetometer();
    //calibrateAccelerometer();
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

int main()
{
    stdio_init_all();

    sleep_ms(5000);
    printf("Starting...\n");

    // gpio_set_dir(BTN_PIN, GPIO_IN);
    // gpio_set_pulls(BTN_PIN, true, false);

    // if (cyw43_arch_init()) {
    //     printf("Wi-Fi init failed.");
    //     return -1;
    // }
    //init everything
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
    while (true) 
    {
 
        //printf("curr Move State: %d\n", currMoveState);
        //printf("curr Mode: %d\n", currMode);
    
        //updateBehaviour();
        //updateMovement();

        // double distance_cm = getCm(&echoReceived,startTime_ultra,endTime_ultra,timeout,&ultrasonicTimeoutReceived);

        updated_duty_cycle = compute_pid(r_speed,l_speed,&integral,&prev_error);
        printf("Modifier: %f\n",updated_duty_cycle);
        duty_cycle += updated_duty_cycle;
        printf("Modified Duty Cycle : %f\n",duty_cycle);
        update_speed(&leftSliceNum,PWM_CHAN_A,duty_cycle);  
        // if (!ultrasonicTimeoutReceived) {
        //     printf("Distance from nearest object: %.2f (cm)\n", distance_cm);
        // } else {
        //     printf("Timeout reached.\n");
        //     ultrasonicTimeoutReceived = false;
        // }
        int16_t x_acc, y_acc, z_acc;
        int16_t x_mag, y_mag, z_mag;

        bool magTimeout; 
        getMagnetometerTimeout(&magTimeout);

        if(!magTimeout) 
        {
            readAccelerometerData(&x_acc, &y_acc, &z_acc);
            readMagnetometerData(&x_mag, &y_mag, &z_mag);
            // convertMagDataToUT(&x_mag, &y_mag, &z_mag);
            calculateAcceleration(x_acc, y_acc, z_acc);
            // printf("Accelerometer Data: (X = %d, Y = %d, Z = %d)\n", x_acc, y_acc, z_acc);
            //printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", x_mag, y_mag, z_mag);

            double compass_bearing = getCompassBearing(x_mag, y_mag);
            //printf("Compass Bearing: %.2f degrees\n", compass_bearing);
        
        }
        else {
            //printf("Magnetometer read timed out.\n");
            //magnetometerTimeoutReceived = false;
            setMagnetometerTimeout(false);
        }

    
        // if(gpio_get(BTN_PIN))
        // {
        //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        //     sleep_ms(250);
        //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        //     sleep_ms(250);
        // }

        // pseudo code
        sleep_ms(150); // 60 fps
    }
}