#include "rpsio.h"
#include "rpslib.h"
#include "gpio.h"
#include "timer.h"
#include "pmap.h"
#include "env.h"

extern u_long get_current_el();
extern void el1_mmu_activate();

extern u_char userA[];
extern u_char userB[];

void main(void)
{
    // activate el1/0 mmu
    el1_mmu_activate();

    _printf("We are now at EL%lx.\n", get_current_el());
    _printf("\n");

    gpio_output_init(17);
    gpio_output_init(27);

    _printf("creating new env...\n");
    env_create(userA, 66816);
    env_create(userB, 66816);

    _printf("activating system timer...\n");
    set_system_timer_irq(1000000);

    _printf("system initialization finished.\n");

    for ( ; ; )
    {
    }

    panic("The end of main()\n");
}
