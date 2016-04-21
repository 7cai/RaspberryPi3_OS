#include "uart.h"
#include "pmap.h"

extern void el1_mmu_activate();
extern void jump_to_el1();

void el3_main(void)
{
    // initialize uart
    uart_init();
    
    // create page dir and page map 
    vm_init();
    
    // fill page map
	page_init();
    
    // activate mmu for el1 
    el1_mmu_activate();
    
    // jump to el1
    jump_to_el1();
}