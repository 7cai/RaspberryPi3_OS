#include "trap.h"
#include "rpsio.h"
#include "rpslib.h"
#include "timer.h"
#include "gpio.h"
#include "sysconfig.h"
#include "mmu.h"
#include "pmap.h"
#include "env.h"
#include "sched.h"

extern u_int get_esr();
extern u_long get_spsr();
extern u_long get_elr();
extern u_long get_far();

extern struct Env *curenv;

void irq_handler()
{
    clr_timer_irq();

    _sched_yield();

    set_system_timer_irq(0);
}


void generic_handler()
{
    panic("Unknown Exception - esr: %x spsr: %lx elr %lx far: %lx\n", get_esr(), get_spsr(), get_elr(), get_far());
}

void mmu_fault_handler()
{
    _printf("MMU Fault - esr: %x spsr: %lx elr %lx far: %lx\n", get_esr(), get_spsr(), get_elr(), get_far());
    
	u_long va = get_far();
    Pte *pte;
    struct Page *page;
    pgdir_walk(curenv->env_ttbr0, va, 0, &pte);
	
	if (!pte || (*pte & ATTRIB_AP_RO_ALL) == 0) {
		panic("It is not a copy-on-write page at va:%d\n", va);
	}

	//map the new page at a temporary place
	page_alloc(&page);
    page_insert(curenv->env_ttbr0, page, USTACKTOP, ATTRIB_AP_RW_ALL);

	//copy the content
    void *tmp;
	tmp = (void *)ROUNDDOWN(va, BY2PG);
	bcopy((void *)tmp, (void *)USTACKTOP, BY2PG);
    
    page_insert(curenv->env_ttbr0, page, (u_long)tmp, ATTRIB_AP_RW_ALL);
    page_remove(curenv->env_ttbr0, USTACKTOP);
    tlb_invalidate();
    _printf("MMU fault fixed\n");
}