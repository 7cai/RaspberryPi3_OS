#include "rpsio.h"
#include "rpslib.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "pmap.h"
#include "env.h"

extern u_long get_current_el();

// extern u_char userA[];
// extern u_long userA_size;
// extern u_char userB[];
// extern u_long userB_size;
extern u_char user[];
extern u_long user_size;

void main(void)
{
    uart_init();
    _printf("\n\nInitializing...\n");
    
    // fill page map
    page_init();
    _printf("page_init success.\n");

    // init env structs
    env_init();
    _printf("env_init success.\n");

    _printf("We are now at EL%lx.\n", get_current_el());
    _printf("\n");

    gpio_output_init(17);
    gpio_output_init(27);

    _printf("creating new env...\n");
    //env_create(userA, userA_size);
    //env_create(userB, userB_size);
    env_create(user, user_size);

    _printf("activating system timer...\n");
    set_system_timer_irq(1000000);

    _printf("system initialization finished.\n");

    for ( ; ; )
    {
    }

    panic("The end of main()\n");
}
