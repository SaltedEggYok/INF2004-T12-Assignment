// #include "pico/stdlib.h"
// #include <stdio.h>
// #include <string.h> 

// const uint LEFT_IR_SENSOR_PIN = 25;
// const uint RIGHT_IR_SENSOR_PIN = 27;
// const uint MIDDLE_IR_SENSOR_PIN = 26;
// const int DEBOUNCE_INTERVAL = 200;  // Can adjust

// bool read_sensor(uint pin) {
//     return !gpio_get(pin);  // Default is LOW
// }

// typedef struct {
//     char* pattern;
//     char value;
// } BarcodeMapping;

// BarcodeMapping mappings[] = {
//     {"311113113", 'A'},
//     {"113113113", 'B'},
//     {"313113111", 'C'},
//     {"111133113", 'D'},
// };

// void interpret_barcode(char *barcode) {
//     int num_mappings = sizeof(mappings) / sizeof(mappings[0]);
//     for (int i = 0; i < num_mappings; i++) {
//         if (strcmp(barcode, mappings[i].pattern) == 0) {
//             printf("Detected barcode: %c\n", mappings[i].value);
//             return;
//         }
//     }
//     printf("Unknown barcode pattern!\n");
// }

// int main() {
//     stdio_init_all();

//     gpio_init(LEFT_IR_SENSOR_PIN);
//     gpio_set_dir(LEFT_IR_SENSOR_PIN, GPIO_IN);
//     gpio_init(RIGHT_IR_SENSOR_PIN);
//     gpio_set_dir(RIGHT_IR_SENSOR_PIN, GPIO_IN);
//     gpio_init(MIDDLE_IR_SENSOR_PIN);
//     gpio_set_dir(MIDDLE_IR_SENSOR_PIN, GPIO_IN);

//     sleep_ms(5000); 
//     printf("Starting to monitor...\n");

//     char barcode[28] = {0};
//     int barcode_index = 0;
//     int64_t startTime = to_ms_since_boot(get_absolute_time());
//     bool last_detected = false;
//     char start_sequence[] = "131131311"; 
//     char end_sequence[] = "131131311"; 

//     while (true) {
//         bool left_detected = read_sensor(LEFT_IR_SENSOR_PIN);
//         bool right_detected = read_sensor(RIGHT_IR_SENSOR_PIN);
//         bool middle_detected = read_sensor(MIDDLE_IR_SENSOR_PIN);

//         int64_t current_time = to_ms_since_boot(get_absolute_time());

//         // Detecting transitions and measuring duration
//         if (middle_detected != last_detected) { 
//             int elapsedTime = current_time - startTime;
//             printf("Elapsed Time: %d ms\n", elapsedTime);
//             if (elapsedTime > 1500) {
//                 if (last_detected) { 
//                     printf("Detected thick black\n");
//                     barcode[barcode_index++] = '3'; 
//                 } else {
//                     printf("Detected thick white\n");
//                     barcode[barcode_index++] = '3'; 
//                 }
//             } else {
//                 if (last_detected) { 
//                     printf("Detected thin black\n");
//                     barcode[barcode_index++] = '1';  
//                 } else {
//                     printf("Detected thin white\n");
//                     barcode[barcode_index++] = '1';  
//                 }
//             }
            
//             // Reset the start time for next duration measurement
//             startTime = current_time;
//             last_detected = middle_detected;
//         }

//         // Checking for barcode start and end sequences
//         if (barcode_index == 9) {
//             if (strncmp(barcode, start_sequence, 9) == 0) {
//                 printf("Barcode reading started!\n");
//             } else if (strncmp(barcode, end_sequence, 9) == 0) {
//                 interpret_barcode(barcode);
//                 barcode_index = 0;
//             } else {
//                 barcode_index = 0; 
//             }
//         }

//         // Side Wall detection
//         if (!left_detected && !right_detected && barcode_index == 0) {
//             printf("Nothing detected!\n");
//         } else if (left_detected && !right_detected) {
//             printf("Left sensor detects something!\n");
//         } else if (!left_detected && right_detected) {
//             printf("Right sensor detects something!\n");
//         } else if (left_detected && right_detected) {
//             printf("Both sensors detect something!\n");
//             //Call go straight function
//         }

//         sleep_ms(500);
//     }

//     return 0;
// }


#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const uint MIDDLE_PIN = 26;

bool isBarcode = true;
bool is_timing = false;


#define MAX_BARCODE_LENGTH 1024
#define DEBOUNCE_TIME_MS 20 // Debounce time in milliseconds

bool barcode_data[MAX_BARCODE_LENGTH];
uint16_t barcode_index = 0;

const uint32_t BARCODE_RESET_INTERVAL = 1000000; 
bool current_state = false;
bool previous_state = false;

void barcode_callback(uint gpio, uint32_t events) {

    if (events & GPIO_IRQ_EDGE_RISE) {
        current_state = true;
    } else if (events & GPIO_IRQ_EDGE_FALL) {
        current_state = false;
    }
}

int main() {
    stdio_init_all();
    gpio_init(MIDDLE_PIN);
    gpio_set_dir(MIDDLE_PIN, GPIO_IN);
    
    uint32_t start_time = 0;
    uint32_t firstBlackWidth = 0;
    uint32_t firstWhiteWidth = 0;


    // Set up a timer for debouncing
    uint32_t debounce_time = time_us_32();                // Set debounce_time to 32 bit timestamp value in microseconds
    uint32_t debounce_interval = DEBOUNCE_TIME_MS * 1000; // Set DEBOUNCE_TIME_MS to microseconds
    
    // Setup the GPIO interrupt for both rising and falling edges
    gpio_set_irq_enabled_with_callback(MIDDLE_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &barcode_callback);

    while (1) {
        
        if(time_us_32() - debounce_time >= debounce_interval)
        {
            current_state = gpio_get(MIDDLE_PIN); 
            debounce_time = time_us_32(); 
        }

        if (isBarcode && !is_timing && current_state == true) {
            printf("Barcode detected. Start scanning\n");
            start_time = time_us_32();
            is_timing = true;
            previous_state = true;
        } 
        else if (isBarcode && current_state != previous_state && is_timing) {
            uint32_t elapsed_time = (time_us_32() - start_time);
            start_time = time_us_32();
            printf("Elapsed Time (s): %d \n", elapsed_time);

            if (firstBlackWidth == 0) {
                firstBlackWidth = elapsed_time;
            }
            else if (firstBlackWidth != 0 && firstWhiteWidth == 0) {
                firstWhiteWidth = elapsed_time;
            }
            //check if thick black
            else if (elapsed_time > (2 * firstBlackWidth) && previous_state == true) {
                printf("THICK black\n");
            }
            //check if skinny white
            else if (((elapsed_time * 2) < firstWhiteWidth) && previous_state == false) {
                printf("SKINNY white\n");
            }
            else if (previous_state == true) {
                printf("SKINNY black\n");
            }
            else if (previous_state == false) {
                printf("THICK white\n");
            }
            if (barcode_index < MAX_BARCODE_LENGTH) {
                barcode_data[barcode_index++] = current_state;
            }
            if (elapsed_time > (2 * firstWhiteWidth) && current_state == true) {
                isBarcode = false; 
                is_timing = false;
                firstBlackWidth = 0; 
                firstWhiteWidth = 0; 
                printf("End of barcode\n"); 
                printf("\n");
                barcode_index = 0;
                isBarcode = true;
            }

            previous_state = current_state;
        }
        sleep_ms(1000);
    }
    return 0;
}