#include <stdio.h>
#include "pico/stdlib.h"
#include "drivers/uart_log.h"
#include "drivers/mpu6050.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define PIN_SDA 26
#define PIN_SCL 27

int main(void)
{
    uart_log_init();
    sleep_ms(2000);
    uart_log("MPU6050 testi basladi");

    mpu6050_status_t st = mpu6050_init(I2C_PORT, PIN_SDA, PIN_SCL);

    if (st != MPU6050_OK)
    {
        while (true)
        {
            uart_log("MPU6050 baslatilamadi, kablolari ve adresi kontrol et");
            sleep_ms(2000);
        }
    }

    while (true)
    {
        mpu6050_raw_t raw;
        mpu6050_data_t d;

        if (mpu6050_read_raw(&raw) == MPU6050_OK)
        {
            mpu6050_convert(&raw, &d);
            uart_log("a[g]= %6.2f %6.2f %6.2f | g[dps]= %7.1f %7.1f %7.1f",
                     d.ax, d.ay, d.az, d.gx, d.gy, d.gz);
        }
        else
        {
            uart_log("okuma hatasi");
        }
        sleep_ms(200);
    }
}