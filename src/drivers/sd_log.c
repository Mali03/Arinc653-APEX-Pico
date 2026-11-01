#include "sd_log.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "ff.h"
#include <stdarg.h>
#include "pico/stdlib.h"

#define LOG_VOLUME "0:"
#define LOG_NAME_FMT "log_%04lu.csv"
#define LOG_HEADER "tick_ms,partition,event,level,data\n"

#define SD_LOG_SYNC_EVERY 10
#define SD_LOG_LINE_MAX 128

static FATFS s_fs;
static FIL s_file;
static bool s_open = false;
static uint32_t s_boot_no = 0;
static uint32_t s_since_sync = 0;

DWORD get_fattime(void)
{
    return ((DWORD)(2026 - 1980) << 25) /* yil */
           | ((DWORD)11 << 21)          /* ay */
           | ((DWORD)1 << 16)           /* gun */
           | ((DWORD)0 << 11)           /* saat */
           | ((DWORD)0 << 5)            /* dakika */
           | ((DWORD)0 >> 1);           /* saniye / 2 */
}

static const char *level_str(sd_log_level_t lvl)
{
    switch (lvl)
    {
    case SD_LOG_LVL_INFO:
        return "INFO";
    case SD_LOG_LVL_WARN:
        return "WARN";
    case SD_LOG_LVL_ERROR:
        return "ERROR";
    case SD_LOG_LVL_FATAL:
        return "FATAL";
    default:
        return "?";
    }
}

/* Kok dizindeki log_XXXX.csv dosyalarini tarar, en buyuk numarayi dondurur. */
static sd_log_status_t scan_max_boot_no(uint32_t *max_no)
{
    DIR dir;
    FILINFO fno;
    uint32_t max = 0;

    if (f_opendir(&dir, LOG_VOLUME "/") != FR_OK)
        return SD_LOG_ERR_DIR;

    for (;;)
    {
        FRESULT fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0)
            break;

        unsigned long n;
        if (sscanf(fno.fname, "log_%lu.csv", &n) == 1 && n > max)
            max = (uint32_t)n;
    }
    f_closedir(&dir);

    *max_no = max;
    return SD_LOG_OK;
}

sd_log_status_t sd_log_init(void)
{
    /* NOT: Mount satiri kutuphane surumune gore degisebilir.
     * Kullandigin surumun simple_example/main.c dosyasindaki f_mount
     * satirini aynen kullan, sadece bu fonksiyonun icinde. */
    if (f_mount(&s_fs, LOG_VOLUME, 1) != FR_OK)
        return SD_LOG_ERR_MOUNT;

    uint32_t max_no = 0;
    sd_log_status_t st = scan_max_boot_no(&max_no);
    if (st != SD_LOG_OK)
        return st;

    s_boot_no = max_no + 1;

    char name[32];
    snprintf(name, sizeof name, LOG_VOLUME "/" LOG_NAME_FMT, (unsigned long)s_boot_no);

    if (f_open(&s_file, name, FA_WRITE | FA_CREATE_NEW) != FR_OK)
        return SD_LOG_ERR_OPEN;
    s_open = true;

    UINT bw;
    if (f_write(&s_file, LOG_HEADER, sizeof(LOG_HEADER) - 1, &bw) != FR_OK ||
        bw != sizeof(LOG_HEADER) - 1)
        return SD_LOG_ERR_WRITE;

    if (f_sync(&s_file) != FR_OK)
        return SD_LOG_ERR_SYNC;

    return SD_LOG_OK;
}

sd_log_status_t sd_log_write(const char *line)
{
    if (!s_open || line == NULL)
        return SD_LOG_ERR_NOT_INIT;

    UINT len = (UINT)strlen(line);
    UINT bw;

    if (f_write(&s_file, line, len, &bw) != FR_OK || bw != len)
        return SD_LOG_ERR_WRITE;
    if (f_write(&s_file, "\n", 1, &bw) != FR_OK || bw != 1)
        return SD_LOG_ERR_WRITE;

    return SD_LOG_OK;
}

sd_log_status_t sd_log_flush(void)
{
    if (!s_open)
        return SD_LOG_ERR_NOT_INIT;
    return (f_sync(&s_file) == FR_OK) ? SD_LOG_OK : SD_LOG_ERR_SYNC;
}

void sd_log_close(void)
{
    if (s_open)
    {
        f_close(&s_file);
        s_open = false;
    }
}

uint32_t sd_log_boot_no(void)
{
    return s_boot_no;
}

sd_log_status_t sd_log_event(const char *partition,
                             const char *event,
                             sd_log_level_t level,
                             const char *fmt, ...)
{
    if (!s_open)
        return SD_LOG_ERR_NOT_INIT;

    char line[SD_LOG_LINE_MAX];
    uint32_t tick_ms = (uint32_t)(time_us_64() / 1000);

    /* 1) sabit kisim: tick,partition,event,level, */
    int n = snprintf(line, sizeof line, "%lu,%s,%s,%s,",
                     (unsigned long)tick_ms,
                     partition ? partition : "-",
                     event ? event : "-",
                     level_str(level));
    if (n < 0 || n >= (int)sizeof line)
        return SD_LOG_ERR_WRITE;

    /* 2) degisken kisim: data (printf bicimi) */
    if (fmt != NULL)
    {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(line + n, sizeof line - (size_t)n, fmt, ap);
        va_end(ap);
    }

    /* 3) yaz */
    sd_log_status_t st = sd_log_write(line);
    if (st != SD_LOG_OK)
        return st;

    /* 4) flush politikasi */
    s_since_sync++;
    if (level >= SD_LOG_LVL_ERROR || s_since_sync >= SD_LOG_SYNC_EVERY)
    {
        s_since_sync = 0;
        return sd_log_flush();
    }
    return SD_LOG_OK;
}