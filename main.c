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

#include "util/irsensor/sensor.h"
#include "util/map/map.h"
#include "util/wifi/wifi.h"
#include "util/magnetometer/magnetometer.h"
#include "util/ultrasonic/ultrasonic.h"
#include "util/motor_controller/motor_controller.h"
#include "util/wheel_encoder/wheel_encoder.h"

// const uint BTN_PIN = 14;
int main() {
    stdio_init_all();
    // gpio_set_dir(BTN_PIN, GPIO_IN);
    // gpio_set_pulls(BTN_PIN, true, false);

    // if (cyw43_arch_init()) {
    //     printf("Wi-Fi init failed.");
    //     return -1;
    // }
    //init everything
    initSensor();
    initMagnetometer();
    initMap();
    initMotorController();
    initUltrasonic();
    initWheelEncoder();
    initWifi();
    

    while (true) 
    {
        

        // if(gpio_get(BTN_PIN))
        // {
        //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        //     sleep_ms(250);
        //     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        //     sleep_ms(250);
        // }
    }
}