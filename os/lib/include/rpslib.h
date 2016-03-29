#ifndef _rpslib_h_
#define _rpslib_h_

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

void sleep(unsigned int millisecond);

void _panic(const char *, int, const char *, ...)
__attribute__((noreturn));

#endif /* _rpslib_h_ */
