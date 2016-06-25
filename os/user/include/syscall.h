#ifndef __SYSCALL_H__
#define __SYSCALL_H__

/*
 *         function              id
 *--------------------------------------------------
 *         putchar               0
 *         printf                1
 *         sleep                 2
 *         getenvid              3
 *         ipc_can_recv          4
 *         ipc_can_send          5
 *         env_destroy           6
 *         fork                  7
 *         yield              	 8
 */

void putchar(unsigned int c);
void printf(const char *fmt, ...);
void sleep(unsigned int millisecond);
int getenvid(void);
void ipc_can_recv(unsigned long *whom, unsigned long dstva, int *perm, int *value);
int ipc_can_send(unsigned long envid, int value, unsigned long srcva, int perm);
void env_destroy(unsigned int envid);
unsigned int fork(void);
void yield(void);

int ipc_send(unsigned long whom, int val, int srcva, int perm);
int ipc_recv(unsigned long *whom, unsigned long dstva, int *perm);

#endif /* __SYSCALL_H__ */
