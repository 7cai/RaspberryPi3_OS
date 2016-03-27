#ifndef _rpsio_h_
#define _rpsio_h_

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

void printf(const char *fmt, ...);

void putchar(unsigned int c);

void _panic(const char *, int, const char *, ...)
__attribute__((noreturn));

#endif /* _rpsio_h_ */
