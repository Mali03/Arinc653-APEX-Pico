#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

#define MPU6050_ADDR 0x68

// Register adresleri
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_WHO_AM_I 0x75

#define MPU6050_WHO_AM_I_VALUE 0x70

#define MPU6050_ACCEL_LSB_PER_G 16384.0f
#define MPU6050_GYRO_LSB_PER_DPS 131.0f

typedef enum
{
    MPU6050_OK = 0,
    MPU6050_ERR_I2C,
    MPU6050_ERR_ID,
    MPU6050_ERR_NOT_INIT
} mpu6050_status_t;

typedef struct
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
} mpu6050_raw_t;

typedef struct
{
    float ax, ay, az;
    float gx, gy, gz;
} mpu6050_data_t;

mpu6050_status_t mpu6050_init(i2c_inst_t *i2c, uint sda, uint scl);

mpu6050_status_t mpu6050_read_raw(mpu6050_raw_t *out);

// Ham değeri fiziksel birime çevirir (g ve derece/saniye (dps))
void mpu6050_convert(const mpu6050_raw_t *raw, mpu6050_data_t *out);

#endif