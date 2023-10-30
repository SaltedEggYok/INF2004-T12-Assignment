#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

const uint trigPin = 0; // GP0
const uint echoPin = 1; // GP1

const int timeout = 26100; // Timeout in microseconds (100ms)

volatile absolute_time_t startTime;
volatile absolute_time_t endTime;
volatile bool echoReceived = false;
volatile bool timeoutReceived = false;

void setupUltrasonicPins() {
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

void triggerUltrasonic() {
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);
}

void echoHandler(uint gpio, uint32_t events) {
    if (gpio == echoPin) {
        if (gpio_get(echoPin) == 1) {
            startTime = get_absolute_time();
        } else {
            endTime = get_absolute_time();
            echoReceived = true;
        }
    }
}

uint64_t getPulse() {    
    while (!echoReceived) {
        if (absolute_time_diff_us(startTime, endTime) > timeout){
            timeoutReceived = true;
        }
        tight_loop_contents();
    }

    return absolute_time_diff_us(startTime, endTime);
}

uint64_t getCm() {
    echoReceived = false;
    triggerUltrasonic();
    return getPulse() / 58; // Speed of sound in air at 20°C is approximately 343 m/s, so 1 cm is roughly 58 microseconds.
}

int main() {
  stdio_init_all();
  setupUltrasonicPins();

  // Setup echo pin interrupt
  gpio_set_irq_enabled_with_callback(echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echoHandler);

  while (1) {
    uint64_t distance_cm = getCm();

    if (!timeoutReceived) {
      printf("Distance: %llu (cm)\n", distance_cm);
    } else {
      printf("Timeout reached.\n");
      timeoutReceived = false;
    }

    sleep_ms(500);
  }

  return 0;
}