#include "uart_log.h"
#include <stdarg.h>
#include <stdio.h>
#include "pico/stdlib.h"

void uart_log_init(void)
{
    stdio_init_all();
}

void uart_log(const char *fmt, ...)
{
    printf("[%8lu] ", (unsigned long)(time_us_64() / 1000));
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\r\n");
}