#include "uart.h"
#include "pmap.h"
#include "rpsio.h"
#include "rpslib.h"

extern void jump_to_el1();
extern u_long get_current_el();

void el2_main(void)
{
    // initialize uart
    uart_init();
    
    // create page dir and page map 
    vm_init();
    
    // fill page map
	page_init();
    
    // jump to el1
    printf("Now in EL%lx. Jumping to EL1...\n", get_current_el());
    jump_to_el1();
    
    panic("failed to jump");
}
