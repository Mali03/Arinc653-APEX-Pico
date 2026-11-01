#ifndef SD_LOG_H
#define SD_LOG_H

#include <stdint.h>

typedef enum
{
    SD_LOG_OK = 0,
    SD_LOG_ERR_NOT_INIT,
    SD_LOG_ERR_MOUNT,
    SD_LOG_ERR_DIR,
    SD_LOG_ERR_OPEN,
    SD_LOG_ERR_WRITE,
    SD_LOG_ERR_SYNC
} sd_log_status_t;

typedef enum
{
    SD_LOG_LVL_INFO = 0,
    SD_LOG_LVL_WARN,
    SD_LOG_LVL_ERROR,
    SD_LOG_LVL_FATAL
} sd_log_level_t;

/* Karti baglar, log_XXXX.csv icin bir sonraki boot numarasini bulur,
 * dosyayi olusturur ve CSV basligini yazar. Bloklayicidir, sadece
 * Logger partition'dan cagrilmali. */
sd_log_status_t sd_log_init(void);

/* Tek satir ekler (sonuna '\n' eklenir). */
sd_log_status_t sd_log_write(const char *line);

/* Tamponu karta yazar (f_sync). Her kayitta degil, periyodik cagir. */
sd_log_status_t sd_log_flush(void);

void sd_log_close(void);

/* Bu acilisin boot numarasi (init'ten sonra gecerli). */
uint32_t sd_log_boot_no(void);

sd_log_status_t sd_log_event(const char *partition,
                             const char *event,
                             sd_log_level_t level,
                             const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));

#endif