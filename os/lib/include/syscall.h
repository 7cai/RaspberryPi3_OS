#ifndef __SYSCALL_H__
#define __SYSCALL_H__

/*
 *         function              id
 *--------------------------------------------------
 *         printf                0
 *         sleep                 1
 */

void printf(const char *fmt, ...);
void sleep(unsigned int millisecond);

#endif /* __SYSCALL_H__ */
