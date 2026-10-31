#include "mpu6050.h"
#include "pico/stdlib.h"

#define I2C_TIMEOUT_US 5000

static i2c_inst_t *s_i2c = NULL;

static bool reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    int r = i2c_write_timeout_us(s_i2c, MPU6050_ADDR, buf, 2, false, I2C_TIMEOUT_US);
    return r == 2;
}

static bool reg_read(uint8_t reg, uint8_t *dst, size_t len)
{
    int r = i2c_write_timeout_us(s_i2c, MPU6050_ADDR, &reg, 1, true, I2C_TIMEOUT_US);
    if (r != 1)
        return false;
    r = i2c_read_timeout_us(s_i2c, MPU6050_ADDR, dst, len, false, I2C_TIMEOUT_US);
    return r == (int)len;
}

mpu6050_status_t mpu6050_init(i2c_inst_t *i2c, uint sda, uint scl)
{
    s_i2c = i2c;

    i2c_init(s_i2c, 400 * 1000); /* 400 kHz fast mode */
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    uint8_t id = 0;
    if (!reg_read(MPU6050_REG_WHO_AM_I, &id, 1))
        return MPU6050_ERR_I2C;
    if (id != MPU6050_WHO_AM_I_VALUE)
        return MPU6050_ERR_ID;

    if (!reg_write(MPU6050_REG_PWR_MGMT_1, 0x00))
        return MPU6050_ERR_I2C;
    sleep_ms(100);

    return MPU6050_OK;
}

mpu6050_status_t mpu6050_read_raw(mpu6050_raw_t *out)
{
    if (s_i2c == NULL)
        return MPU6050_ERR_NOT_INIT;

    uint8_t b[14];
    if (!reg_read(MPU6050_REG_ACCEL_XOUT_H, b, sizeof b))
        return MPU6050_ERR_I2C;

    out->ax = (int16_t)((b[0] << 8) | b[1]);
    out->ay = (int16_t)((b[2] << 8) | b[3]);
    out->az = (int16_t)((b[4] << 8) | b[5]);
    out->gx = (int16_t)((b[8] << 8) | b[9]);
    out->gy = (int16_t)((b[10] << 8) | b[11]);
    out->gz = (int16_t)((b[12] << 8) | b[13]);
    return MPU6050_OK;
}

void mpu6050_convert(const mpu6050_raw_t *r, mpu6050_data_t *d)
{
    d->ax = r->ax / MPU6050_ACCEL_LSB_PER_G;
    d->ay = r->ay / MPU6050_ACCEL_LSB_PER_G;
    d->az = r->az / MPU6050_ACCEL_LSB_PER_G;

    d->gx = r->gx / MPU6050_GYRO_LSB_PER_DPS;
    d->gy = r->gy / MPU6050_GYRO_LSB_PER_DPS;
    d->gz = r->gz / MPU6050_GYRO_LSB_PER_DPS;
}