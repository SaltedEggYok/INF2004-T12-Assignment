#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "magnetometer.h"

int16_t bias_x = 0;
int16_t bias_y = 0;
int16_t bias_z = 0;

// linking with main_lib.h
volatile bool *magnetometerTimeoutReceived_ptr = NULL;
//linking with main_lib.h
volatile double *compassBearing_ptr = NULL;

void initI2C()
{
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    i2c_set_slave_mode(I2C_PORT, false, 0);
}

void writeRegister(uint8_t address, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, address, data, 2, true);
}

uint8_t readRegister(uint8_t address, uint8_t reg)
{
    uint8_t data;
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, &data, 1, false);
    return data;
}

void configureAccelerometer()
{
    writeRegister(ACC_ADDRESS, CTRL_REG1_A, 0x47);
}

void readAccelerometerData(int16_t *x, int16_t *y, int16_t *z)
{
    *x = (int16_t)((readRegister(ACC_ADDRESS, OUT_X_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_X_L_A)) - bias_x;
    *y = (int16_t)((readRegister(ACC_ADDRESS, OUT_Y_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_Y_L_A)) - bias_y;
    *z = (int16_t)((readRegister(ACC_ADDRESS, OUT_Z_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_Z_L_A)) - bias_z;
}

void configureMagnetometer()
{
    writeRegister(MAG_ADDRESS, MR_REG_M, CRA_REG_M);
}

void readMagnetometerData(int16_t *x, int16_t *y, int16_t *z)
{
    *x = (int16_t)((readRegister(MAG_ADDRESS, OUT_X_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_X_L_M));
    *y = (int16_t)((readRegister(MAG_ADDRESS, OUT_Y_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_Y_L_M));
    *z = (int16_t)((readRegister(MAG_ADDRESS, OUT_Z_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_Z_L_M));

    if ((*x == -16192 && *y == -16192 && *z == -16192) ||
        (*x == 16448 && *y == 16448 && *z == 16448) ||
        (*x == -32640 && *y == -32640 && *z == -32640) ||
        (*x == 0 && *y == 0 && *z == 0))
    {
        *magnetometerTimeoutReceived_ptr = true;
    }
}

void calibrateAccelerometer()
{
    const int numSamples = 100;
    int16_t x_accum = 0;
    int16_t y_accum = 0;
    int16_t z_accum = 0;

    for (int i = 0; i < numSamples; i++)
    {
        int16_t x, y, z;
        readAccelerometerData(&x, &y, &z);
        x_accum += x;
        y_accum += y;
        z_accum += z;
        sleep_ms(10);
    }

    bias_x = x_accum / numSamples;
    bias_y = y_accum / numSamples;
    bias_z = z_accum / numSamples;
}

void calculateAcceleration(int16_t x, int16_t y, int16_t z)
{
    double g = 9.81; // Standard gravity in m/s^2
    double acc_x = (double)x * (g / 16384.0);
    double acc_y = (double)y * (g / 16384.0);
    double acc_z = (double)z * (g / 16384.0);

    // printf("Acceleration: (X = %.2f m/s^2, Y = %.2f m/s^2, Z = %.2f m/s^2)\n", acc_x, acc_y, acc_z);
}

// Magnetic Field formula : raw data x magnetic cross-axis sensitivity
// void convertMagDataToUT(int16_t* x, int16_t* y, int16_t* z) {
//     *x = (int16_t)(*x / MAG_SENSITIVITY);
//     *y = (int16_t)(*y / MAG_SENSITIVITY);
//     *z = (int16_t)(*z / MAG_SENSITIVITY);
// }

double getCompassBearing(int16_t x, int16_t y)
{
    double angle = atan2((double)y, (double)x);
    if (angle < 0)
    {
        angle += 2 * M_PI;
    }
    return (angle * 180.0) / M_PI;
}

void initMagnetometer(volatile bool *magnetometerTimeoutReceived, volatile double* compassBearing)
{
    // linking pointers
    magnetometerTimeoutReceived_ptr = magnetometerTimeoutReceived;
    compassBearing_ptr = compassBearing;

    initI2C();
    configureAccelerometer();
    configureMagnetometer();
    calibrateAccelerometer();
}


void magnetometerTask(__unused void *params)
{
    float fps = 5;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;

    while (true)
    {
        printf("magnetometer task\n"); //DEBUG
        int16_t x_acc, y_acc, z_acc;
        int16_t x_mag, y_mag, z_mag;

        if (!*magnetometerTimeoutReceived_ptr)
        {
            readAccelerometerData(&x_acc, &y_acc, &z_acc);
            readMagnetometerData(&x_mag, &y_mag, &z_mag);
            // convertMagDataToUT(&x_mag, &y_mag, &z_mag);
            calculateAcceleration(x_acc, y_acc, z_acc);
            // printf("Accelerometer Data: (X = %d, Y = %d, Z = %d)\n", x_acc, y_acc, z_acc);
            printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", x_mag, y_mag, z_mag);

            *compassBearing_ptr = getCompassBearing(x_mag, y_mag);
            printf("Compass Bearing: %.2f degrees\n", *compassBearing_ptr);
        }
        else
        {
            printf("Magnetometer read timed out.\n");
            *magnetometerTimeoutReceived_ptr = false;
        }

        vTaskDelay(frame_time);
    }
}

// int main() {
//     stdio_init_all();
//     initMagnetometer();

//     while (1) {
//         int16_t x_acc, y_acc, z_acc;
//         int16_t x_mag, y_mag, z_mag;

//         if(!magnetometerTimeoutReceived) {
//             readAccelerometerData(&x_acc, &y_acc, &z_acc);
//             readMagnetometerData(&x_mag, &y_mag, &z_mag);
//             // convertMagDataToUT(&x_mag, &y_mag, &z_mag);
//             calculateAcceleration(x_acc, y_acc, z_acc);
//             // printf("Accelerometer Data: (X = %d, Y = %d, Z = %d)\n", x_acc, y_acc, z_acc);
//             printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", x_mag, y_mag, z_mag);

//             double compass_bearing = getCompassBearing(x_mag, y_mag);
//             printf("Compass Bearing: %.2f degrees\n", compass_bearing);

//         }
//         else {
//             printf("Magnetometer read timed out.\n");
//             magnetometerTimeoutReceived = false;
//         }

//         sleep_ms(500);
//     }

//     return 0;
// }
