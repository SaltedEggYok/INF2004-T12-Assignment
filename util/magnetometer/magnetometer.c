#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define I2C_PORT i2c0

#define ACC_ADDRESS 0x19
#define MAG_ADDRESS 0x1E

#define CTRL_REG1_A 0x20
#define STATUS_REG_A 0x27
#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D

#define CRA_REG_M 0x00
#define MR_REG_M 0x02
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Z_H_M 0x05
#define OUT_Z_L_M 0x06
#define OUT_Y_H_M 0x07
#define OUT_Y_L_M 0x08

#define PIN_SDA 0
#define PIN_SCL 1

int16_t bias_x = 0;
int16_t bias_y = 0;
int16_t bias_z = 0;

void initI2C() {
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    i2c_set_slave_mode(I2C_PORT, false, 0);
}

void writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, address, data, 2, true);
}

uint8_t readRegister(uint8_t address, uint8_t reg) {
    uint8_t data;
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, &data, 1, false);
    return data;
}

void configureAccelerometer() {
    writeRegister(ACC_ADDRESS, CTRL_REG1_A, STATUS_REG_A);
}

void readAccelerometerData(int16_t* x, int16_t* y, int16_t* z) {
    *x = (int16_t)((readRegister(ACC_ADDRESS, OUT_X_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_X_L_A)) - bias_x;
    *y = (int16_t)((readRegister(ACC_ADDRESS, OUT_Y_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_Y_L_A)) - bias_y;
    *z = (int16_t)((readRegister(ACC_ADDRESS, OUT_Z_H_A) << 8) | readRegister(ACC_ADDRESS, OUT_Z_L_A)) - bias_z;
}

void configureMagnetometer() {
    writeRegister(MAG_ADDRESS, MR_REG_M, CRA_REG_M);
}

void readMagnetometerData(int16_t* x, int16_t* y, int16_t* z) {
    *x = (int16_t)((readRegister(MAG_ADDRESS, OUT_X_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_X_L_M));
    *y = (int16_t)((readRegister(MAG_ADDRESS, OUT_Y_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_Y_L_M));
    *z = (int16_t)((readRegister(MAG_ADDRESS, OUT_Z_H_M) << 8) | readRegister(MAG_ADDRESS, OUT_Z_L_M));
}

void calibrateAccelerometer() {
    const int numSamples = 100;  
    int16_t x_accum = 0;
    int16_t y_accum = 0;
    int16_t z_accum = 0;

    for (int i = 0; i < numSamples; i++) {
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

void calculateAcceleration(int16_t x, int16_t y, int16_t z) {
    double g = 9.81; // Standard gravity in m/s^2
    double acc_x = (double)x * (g / 16384.0);
    double acc_y = (double)y * (g / 16384.0);
    double acc_z = (double)z * (g / 16384.0);

    printf("Acceleration: (X = %.2f m/s^2, Y = %.2f m/s^2, Z = %.2f m/s^2)\n", acc_x, acc_y, acc_z);
}

int main() {
    stdio_init_all();
    initI2C();
    configureAccelerometer();
    configureMagnetometer();
    calibrateAccelerometer();

    while (1) {
        int16_t x_acc, y_acc, z_acc;
        int16_t x_mag, y_mag, z_mag;

        readAccelerometerData(&x_acc, &y_acc, &z_acc);
        readMagnetometerData(&x_mag, &y_mag, &z_mag);

        calculateAcceleration(x_acc, y_acc, z_acc);
        // printf("Accelerometer Data: (X = %d, Y = %d, Z = %d)\n", x_acc, y_acc, z_acc);
        printf("Magnetometer Data: (X = %d, Y = %d, Z = %d)\n", x_mag, y_mag, z_mag);

        sleep_ms(500);
    }

    return 0;
}
