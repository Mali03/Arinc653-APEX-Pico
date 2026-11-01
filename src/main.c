#include <stdio.h>
#include "pico/stdlib.h"
#include "drivers/uart_log.h"
#include "drivers/mpu6050.h"
#include "drivers/sd_log.h"

#define I2C_PORT i2c1
#define PIN_SDA 26
#define PIN_SCL 27

#define SENSOR_PERIOD_MS 200
#define SD_LOG_EVERY_N 5 // her 5 okumada (1 Hz) bir SD'ye kayit

int main(void)
{
    uart_log_init();
    sleep_ms(2000);
    uart_log("Sistem baslatiliyor");

    // SD kart: basarisiz olursa sistem calismaya devam eder (hata izolasyonu)
    bool sd_ok = (sd_log_init() == SD_LOG_OK);
    if (sd_ok)
    {
        uart_log("SD log hazir, boot=%lu", (unsigned long)sd_log_boot_no());
        sd_log_event("SYS", "BOOT", SD_LOG_LVL_INFO, "boot=%lu",
                     (unsigned long)sd_log_boot_no());
    }
    else
    {
        uart_log("SD log baslatilamadi, SD'siz devam ediliyor");
    }

    // Sensor: bu sistemin temel girdisi, olmadan devam etmenin anlami yok
    mpu6050_status_t st = mpu6050_init(I2C_PORT, PIN_SDA, PIN_SCL);
    if (st != MPU6050_OK)
    {
        if (sd_ok)
            sd_log_event("SENSOR", "INIT_FAIL", SD_LOG_LVL_FATAL, "code=%d", (int)st);
        while (true)
        {
            uart_log("MPU6050 baslatilamadi (kod=%d)", (int)st);
            sleep_ms(2000);
        }
    }
    uart_log("MPU6050 hazir");
    if (sd_ok)
        sd_log_event("SENSOR", "INIT_OK", SD_LOG_LVL_INFO, "");

    // Sensor Verisi Oku
    absolute_time_t next = get_absolute_time();
    uint32_t count = 0;

    while (true)
    {
        mpu6050_raw_t raw;
        mpu6050_data_t d;

        if (mpu6050_read_raw(&raw) == MPU6050_OK)
        {
            mpu6050_convert(&raw, &d);
            uart_log("a[g]= %6.2f %6.2f %6.2f | g[dps]= %7.1f %7.1f %7.1f",
                     d.ax, d.ay, d.az, d.gx, d.gy, d.gz);

            if (sd_ok && (++count % SD_LOG_EVERY_N) == 0)
                sd_log_event("SENSOR", "SAMPLE", SD_LOG_LVL_INFO,
                             "ax=%d ay=%d az=%d", raw.ax, raw.ay, raw.az);
        }
        else
        {
            uart_log("okuma hatasi");
            if (sd_ok)
                sd_log_event("SENSOR", "I2C_ERR", SD_LOG_LVL_ERROR, "");
        }

        // Kayma olmadan sabit periyot: bir onceki hedef zamana gore bekler (sleep_ms'de iş 7 ms sürerse 200 + 7 ms döngü sürer ama bu kullanımda sabit 200 ms sürer)
        next = delayed_by_ms(next, SENSOR_PERIOD_MS);
        sleep_until(next);
    }
}