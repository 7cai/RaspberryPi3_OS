#ifndef _TRAP_H_
#define _TRAP_H_

/* these are processor defined */
#define T_DIVIDE  0      /* divide error */
#define T_DEBUG   1      /* debug exception */
#define T_NMI     2      /* non-maskable interrupt */
#define T_BRKPT   3      /* breakpoint */
#define T_OFLOW   4      /* overflow */
#define T_BOUND   5      /* bounds check */
#define T_ILLOP   6      /* illegal opcode */
#define T_DEVICE  7      /* device not available */
#define T_DBLFLT  8      /* double fault */
/* 9 is reserved */
#define T_TSS     10     /* invalid task switch segment */
#define T_SEGNP   11     /* segment not present */
#define T_STACK   12     /* stack exception */
#define T_GPFLT   13     /* genernal protection fault */
#define T_PGFLT   14     /* page fault */
/* 15 is reserved */
#define T_FPERR   16     /* floating point error */
#define T_ALIGN   17     /* aligment check */
#define T_MCHK    18     /* machine check */

/* These are arbitrarily chosen, but with care not to overlap
 * processor defined exceptions or interrupt vectors.
 */
#define T_SYSCALL 0x30   /* system call */
#define T_DEFAULT 500    /* catchall */

#include <types.h>

struct Trapframe
{
    /* Saved special registers. */
    u_long spsr;
    u_long elr;
    u_long esr;
    u_long sp;
    
    /* Saved main processor registers. */
    u_long regs[31];
};
void *set_except_vector(int n, void *addr);
void trap_init();
#endif /* _TRAP_H_ */
