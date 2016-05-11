#include "sysconfig.h"

.globl _start
_start:
    mrs x0, mpidr_el1       // check core id, we only use one core.
    mov x1, #0xc1000000
    bic x0, x0, x1
    cbz x0, master
    b hang

master:
    ldr x0, =0x1000000
    mov sp, x0               // set el3 sp
    bl el2_main

hang:
    b hang

.globl get_current_el
get_current_el:
    mrs x0, currentel
    mov x1, #2
    lsr x0, x0, #2
    ret

.globl el1_mmu_activate
el1_mmu_activate:
    ldr x0, =0x04cc
    msr mair_el1, x0
    isb

    ldr x1, =0x02000000
    msr ttbr0_el1, x1
    isb

    mrs x2, tcr_el1
    ldr x3, =0x70040ffbf
    bic x2, x2, x3

    ldr x3, =0x200803f18
    orr x2, x2, x3
    msr tcr_el1, x2
    isb

    mrs x3, sctlr_el1
    ldr x4, =0x80000
    bic x3, x3, x4

    ldr x4, =0x1005
    orr x3, x3, x4
    msr sctlr_el1, x3
    isb
    ret

.globl jump_to_el1
jump_to_el1:
	mrs	x0, currentel		// check if already in el1
	cmp	x0, #4
	beq	1f

	ldr	x0, =0xf00000
	msr	sp_el1, x0	        // init the stack of el1

    // disable coprocessor traps
	mov	x0, #0x33ff
	msr	cptr_el2, x0	    // disable coprocessor traps to el2
	msr	hstr_el2, xzr		// disable coprocessor traps to el2
	mov	x0, #3 << 20
	msr	cpacr_el1, x0	    // enable fp/simd at el1

	// initialize hcr_el2
	mov	x0, #(1 << 31)
	msr	hcr_el2, x0		    // set el1 to 64 bit
	mov	x0, #0x0800
	movk x0, #0x30d0, lsl #16
	msr	sctlr_el1, x0

	// return to the el1_sp1 mode from el2
	mov	x0, #0x3c5
	msr	spsr_el2, x0	    // el1_sp0 | d | a | i | f
	adr	x0, 1f
	msr	elr_el2, x0
	eret

1:
	mrs	x0, sctlr_el1
	orr	x0, x0, #(1 << 12)
	msr	sctlr_el1, x0		// enable instruction cache
    b main

.globl tlb_invalidate
tlb_invalidate:
    dsb ishst               // ensure write has completed
    ldr x0, [x0]            // load va from x0
    tlbi vmalle1            // invalidate tlb by va, all asid, el1.
    dsb ish                 // ensure completion of tlb invalidation
    isb                     // synchronize context and ensure that no instructions
                            // are fetched using the old translation
    ret

.globl put32
put32:
    str w1,[x0]
    ret

.globl get32
get32:
    ldr w0,[x0]
    ret

.globl dummy
dummy:
    ret
