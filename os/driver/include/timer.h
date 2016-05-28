#ifndef _timer_h_
#define _timer_h_

#include "types.h"

#define TIMER_FREQUENCY 1000000     // 1MHZ

u_long get_system_timer();

void set_system_timer_irq(u_long delay);

void clr_timer_irq();

#endif /* _timer_h_ */
