#ifndef _SWUART_H_
#define _SWUART_H_

#include <stdio.h>

#define SWUART_BAUD(baud, f) ((f)/(baud*64l))

void swuart_init(unsigned int btime, FILE *stream);

#endif
