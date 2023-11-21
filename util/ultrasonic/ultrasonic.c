#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ultrasonic.h"

// used to track for ultrasonic timeout
volatile absolute_time_t startTime;
volatile absolute_time_t endTime;
const int timeout = 26100; // Timeout in microseconds (100ms)

// echoReceived will be set to true in the callback,
// on edge fall
volatile bool echoReceived = false;

// will be linked to main_lib.h
volatile bool *ultrasonicTimeoutReceived_ptr = false;
// will be linked to main_lib.h
volatile float *distanceCM_ptr = NULL;

void setupUltrasonicPins()
{
    gpio_init(ULTRASONIC_TRIG_PIN);
    gpio_init(ULTRASONIC_ECHO_PIN);
    gpio_set_dir(ULTRASONIC_TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ULTRASONIC_ECHO_PIN, GPIO_IN);
}

void triggerUltrasonic()
{
    gpio_put(ULTRASONIC_TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(ULTRASONIC_TRIG_PIN, 0);
}

// the callback function for when ultrasonic echo pin rises or falls
void ultrasonicCallback(uint gpio, uint32_t events)
{
    if (gpio == ULTRASONIC_ECHO_PIN)
    {
        // on edge rise, signal sent out
        if (gpio_get(ULTRASONIC_ECHO_PIN) == 1)
        {
            startTime = get_absolute_time();
        }
        // on edge fall, signal returned
        else
        {
            endTime = get_absolute_time();
            echoReceived = true;
        }
    }
}

// task specifically assigned to ultrasonic
void ultrasonicTask(__unused void *params)
{
    float fps = 30;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;

    // this task will poll the ultrasonic a few times every second
    // and update the distanceCM variable, so the pointer from outside is also updated
    while (true)
    {
        printf("ultrasonic task\n"); // DEBUG
        if (!*ultrasonicTimeoutReceived_ptr)
        {
            // get distance in cm
            *distanceCM_ptr = getCM();
            printf("Distance: %.2f (cm)\n", *distanceCM_ptr); // DEBUG
            echoReceived = false;
        }
        else
        {
            printf("Ultrasonic read timed out.\n");
            *ultrasonicTimeoutReceived_ptr = false;
        }
        vTaskDelay(frame_time);
    }
}

float getCM()
{
    echoReceived = false;
    // Trigger the ultrasonic burst
    triggerUltrasonic();

    // Speed of sound in air at 20°C is approximately 343 m/s, so 1 cm is roughly 58 microseconds.
    return getPulse(echoReceived) / 58.0f;
}

float getPulse()
{
    // wait for echo to be received, then return the time difference between the start and end time
    while (!echoReceived)
    {
        printf("waiting for pulse \n"); // DEBUG
        if (absolute_time_diff_us(startTime, endTime) > timeout)
        {
            *ultrasonicTimeoutReceived_ptr = true;
        }
        sleep_ms(50);
    }

    // return the time difference between the start and end time
    return (float)absolute_time_diff_us(startTime, endTime);
}

// init function for ultrasonic
void initUltrasonic(volatile bool *ultrasonicTimeoutReceived, volatile float *distanceCM)
{
    // linking pointers
    ultrasonicTimeoutReceived_ptr = ultrasonicTimeoutReceived;
    distanceCM_ptr = distanceCM;

    startTime = get_absolute_time();
    endTime = get_absolute_time();
    // init pins
    setupUltrasonicPins();
}

// int main() {
//   stdio_init_all();
//   //setupUltrasonicPins();

//   // Setup echo pin interrupt
//   //gpio_set_irq_enabled_with_callback(echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ultrasonicCallback);

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