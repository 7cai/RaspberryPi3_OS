#include "uart.h"
#include "pmap.h"
#include "rpsio.h"
#include "rpslib.h"

extern void el1_mmu_activate();
extern void jump_to_el1();
extern u_long get_current_el();

void el3_main(void)
{
    // initialize uart
    uart_init();
    
    // create page dir and page map 
    vm_init();
    
    // fill page map
	page_init();
    
    // activate mmu for el1 
//    el1_mmu_activate();
    printf("%lx\n", get_current_el());
    
    // jump to el1
    printf("Jumping to EL1...\n");
    jump_to_el1();
    
    panic("failed to jump");
}