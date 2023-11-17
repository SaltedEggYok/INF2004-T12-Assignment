#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "ultrasonic.h"

// const uint trigPin = 0; // GP0
// const uint echoPin = 1; // GP1

// volatile absolute_time_t startTime;
// volatile absolute_time_t endTime;

// const int timeout = 26100; // Timeout in microseconds (100ms)
// volatile bool echoReceived = false;
// volatile bool ultrasonicTimeoutReceived = false;

void setupUltrasonicPins() {
    gpio_init(ULTRASONIC_TRIG_PIN);
    gpio_init(ULTRASONIC_ECHO_PIN);
    gpio_set_dir(ULTRASONIC_TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ULTRASONIC_ECHO_PIN, GPIO_IN);
}

void triggerUltrasonic() {
    gpio_put(ULTRASONIC_TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(ULTRASONIC_TRIG_PIN, 0);
}

// void echoHandler(uint gpio, uint32_t events) {
//     if (gpio == ULTRASONIC_ECHO_PIN) {
//         if (gpio_get(ULTRASONIC_ECHO_PIN) == 1) {
//             startTime = get_absolute_time();
//         } else {
//             endTime = get_absolute_time();
//             echoReceived = true;
//         }
//     }
// }

// float getPulse() {    
//     while (!echoReceived) {
//         if (absolute_time_diff_us(startTime, endTime) > timeout){
//             ultrasonicTimeoutReceived = true;
//         }
//         tight_loop_contents();
//     }

//     return (float)absolute_time_diff_us(startTime, endTime);
// }

// float getCm() {
//     echoReceived = false;
//     triggerUltrasonic();
//     return getPulse() / 58.0f; // Speed of sound in air at 20°C is approximately 343 m/s, so 1 cm is roughly 58 microseconds.
// }

void initUltrasonic()
{
    setupUltrasonicPins();
    //gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echoHandler);
}

// int main() {
//   stdio_init_all();
//   //setupUltrasonicPins();

//   // Setup echo pin interrupt
//   //gpio_set_irq_enabled_with_callback(echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echoHandler);

//   while (1) {
//         float distance_cm = getCm();

//         if (!ultrasonicTimeoutReceived) {
//             printf("Distance: %.2f (cm)\n", distance_cm);
//         } else {
//             printf("Timeout reached.\n");
//             ultrasonicTimeoutReceived = false;
//         }

//         sleep_ms(500);
//   }

//   return 0;
// }