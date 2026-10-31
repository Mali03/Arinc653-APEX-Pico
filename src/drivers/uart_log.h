#ifndef UART_LOG_H
#define UART_LOG_H

void uart_log_init(void);
void uart_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif