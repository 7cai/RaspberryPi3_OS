#ifndef _console_h_
#define _console_h_

void uart_init (void);

unsigned int uart_recv(void);

void uart_send (unsigned int c);

void uart_flush(void);

#endif