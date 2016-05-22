#include "timer.h"

extern void dummy();

void _sleep(unsigned int millisecond)
{
    u_long time = get_system_timer() + millisecond * 1000;

    while (get_system_timer() < time)
    {
        dummy(time);
    }

    int i;
    for (i = 0; i < 150; i++)
    {
        dummy(time);
    }
}
