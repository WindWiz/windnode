#define UART_BAUD(baud, f) ((f)/((baud)*16l)-1)

void uart_init(unsigned int ubrr);
