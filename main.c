/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "main_lib.h"

#include "util/irsensor/sensor.h"
#include "util/map/map.h"
#include "util/wifi/wifi.h"
#include "util/magnetometer/magnetometer.h"
#include "util/ultrasonic/ultrasonic.h"
#include "util/motor_controller/motor_controller.h"
#include "util/wheel_encoder/wheel_encoder.h"

// init everything
void initAll()
{

    // init local variables
    currMode = DEFAULT;
    currMoveState = STATIONARY;
    leftSensor = rightSensor = false;

    initSensor();
    initMagnetometer();
    initMap();
    initMotorController(&leftSliceNum, &rightSliceNum, &direction);
    initUltrasonic();
    initWheelEncoder();
    initWifi(&wifiEnabled);
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

    // gpio_set_dir(BTN_PIN, GPIO_IN);
    // gpio_set_pulls(BTN_PIN, true, false);

    // if (cyw43_arch_init()) {
    //     printf("Wi-Fi init failed.");
    //     return -1;
    // }

    // init everything
    initAll();

    // main loop
    while (true)
    {

        updateBehaviour();
        updateMovement();

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