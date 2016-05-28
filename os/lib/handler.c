#include "trap.h"
#include "rpsio.h"
#include "rpslib.h"
#include "timer.h"
#include "gpio.h"
#include "sysconfig.h"
#include "sched.h"

extern u_int get_esr();

void irq_handler()
{
    clr_timer_irq();

    sched_yield();

    set_system_timer_irq(0);
}


void generic_handler()
{
    panic("Unknown Exception - esr: %x", get_esr());
}
