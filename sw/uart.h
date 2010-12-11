#ifndef _UART_H_
#define _UART_H_

#define UART_BAUD(baud, f) ((f)/((baud)*16l)-1)

#include <stdio.h>

typedef int (*uart_orun_handler_t)(void);

int uart_init(unsigned int ubrr, FILE *stream, uart_orun_handler_t orun);
void uart_free(void);

#endif
