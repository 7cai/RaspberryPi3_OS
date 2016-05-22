#include "uart.h"
#include "pmap.h"
#include "env.h"
#include "rpsio.h"
#include "rpslib.h"

extern void jump_to_el1();
extern u_long get_current_el();

void el2_main(void)
{
    // initialize uart
    uart_init();
    _printf("\n\nInitializing...\n");

    // create page dir and page map
    vm_init();
    _printf("vm_init success.\n");

    // fill page map
    page_init();
    _printf("page_init success.\n");

    // init env structs
    env_init();
    _printf("env_init success.\n");

    // jump to el1
    _printf("\n");
    _printf("Now in EL%lx. Jumping to EL1...\n", get_current_el());
    jump_to_el1();

    panic("failed to jump EL");
}
