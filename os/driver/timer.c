#include "timer.h"

#define PHYSADDR_OFFSET    0xFFFFFF0000000000uL + 0x3F000000

#define ARM_SYSTIMER_BASE  (PHYSADDR_OFFSET + 0x3000)

#define ARM_SYSTIMER_CS    (ARM_SYSTIMER_BASE + 0x00)
#define ARM_SYSTIMER_CLO   (ARM_SYSTIMER_BASE + 0x04)
#define ARM_SYSTIMER_CHI   (ARM_SYSTIMER_BASE + 0x08)
#define ARM_SYSTIMER_C0    (ARM_SYSTIMER_BASE + 0x0C)
#define ARM_SYSTIMER_C1    (ARM_SYSTIMER_BASE + 0x10)
#define ARM_SYSTIMER_C2    (ARM_SYSTIMER_BASE + 0x14)
#define ARM_SYSTIMER_C3    (ARM_SYSTIMER_BASE + 0x18)

#define ARM_INTERRUPT_BASE (PHYSADDR_OFFSET + 0xB200)
#define ENABLE_IRQ_1       (ARM_INTERRUPT_BASE + 0x010)

u_long get_system_timer()
{
    u_long time = 0;
    time += (*(u_int *)ARM_SYSTIMER_CHI);
    time <<= 32;
    time += (*(u_int *)ARM_SYSTIMER_CLO);
    return time;
}


void set_system_timer_irq(u_long delay)
{
    static u_long saved_delay;
    if (delay)
    {
        saved_delay = delay;
    }

    *(u_int *)ENABLE_IRQ_1 = 1 << 3;
    *(u_int *)ARM_SYSTIMER_C3 = saved_delay + *(u_int *)ARM_SYSTIMER_CLO;
}


void clr_timer_irq()
{
    *(u_int *)ARM_SYSTIMER_CS = 1 << 3;
}
