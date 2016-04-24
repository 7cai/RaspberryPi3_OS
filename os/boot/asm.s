#include "sysconfig.h"

.globl _start
_start:
    MRS X0, MPIDR_EL1       // Check Core Id, we only use one core. 
    MOV X1, #0XC1000000
    BIC X0, X0, X1
    CBZ X0, master
    B hang

master:
    LDR X0, =0X04008000
    MOV SP, X0               // Set EL3 SP
    BL el2_main

hang:
    B hang
    
.globl get_current_el
get_current_el:
    MRS X0, CURRENTEL
    MOV X1, #2
    LSR X0, X0, #2
    RET
    
.globl el1_mmu_activate
el1_mmu_activate:
    LDR X0, =0X04CC
    MSR MAIR_EL1, X0
    ISB

    LDR X1, =0X02000000
    MSR TTBR0_EL1, X1
    ISB

    MRS X2, TCR_EL1
    LDR X3, =0x70040FFBF
    BIC X2, X2, X3

    LDR X3, =0X200803F18
    ORR X2, X2, X3
    MSR TCR_EL1, X2
    ISB

    MRS X3, SCTLR_EL1
    LDR X4, =0X80000
    BIC X3, X3, X4
    
    LDR X4, =0X1005
    ORR X3, X3, X4
    MSR SCTLR_EL1, X3
    ISB
    RET

.globl jump_to_el1
jump_to_el1:
	MRS	X0, CURRENTEL		// Check if already in EL1
	CMP	X0, #4
	BEQ	1f

	LDR	X0, =0X03C08000
	MSR	SP_EL1, X0	        // Init the stack of EL1

    // Disable coprocessor traps
	MOV	X0, #0X33ff
	MSR	CPTR_EL2, X0	    // Disable coprocessor traps to EL2
	MSR	HSTR_EL2, xzr		// Disable coprocessor traps to EL2
	MOV	X0, #3 << 20
	MSR	CPACR_EL1, X0	    // Enable FP/SIMD at EL1

	// Initialize HCR_EL2
	MOV	X0, #(1 << 31)
	MSR	HCR_EL2, X0		    // Set EL1 to 64 bit
	MOV	X0, #0X0800
	MOVK X0, #0X30d0, LSL #16
	MSR	SCTLR_EL1, X0

	// Return to the EL1_SP1 mode from EL2
	MOV	X0, #0X3C5
	MSR	SPSR_EL2, X0	    // EL1_SP0 | D | A | I | F
	ADR	X0, 1f
	MSR	ELR_EL2, X0
	ERET
    
1:
	MRS	X0, SCTLR_EL1
	ORR	X0, X0, #(1 << 12)
	MSR	SCTLR_EL1, X0		// enable instruction cache

    B main
    
.globl tlb_invalidate
tlb_invalidate:
    DSB ISHST               // ensure write has completed
    LDR X0, [X0]            // load VA from X0
    TLBI VAAE1, X0          // invalidate TLB by VA, All ASID, EL1.
    DSB ISH                 // ensure completion of TLB invalidation
    ISB                     // synchronize context and ensure that no instructions are fetched using the old translation
    RET

.globl PUT32
PUT32:
    STR W1,[X0]
    RET

.globl GET32
GET32:
    LDR W0,[X0]
    RET

.globl dummy
dummy:
    RET
