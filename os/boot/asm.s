.globl _start
_start:
    B skip

.space 0x8000 - 0x4, 0

skip:
    MRS X0, mpidr_el1
    MOV X1, #0xC1000000
    BIC X0, X0, X1
    CBZ X0, master
    B hang

master:
    LDR X0, =0x04008000
    MOV SP, X0
    BL el3_main

hang:
    B hang
    
.globl el1_mmu_activate
el1_mmu_activate:
    LDR X0, =0x04008000     // Set TTBR0
    MSR TTBR0_EL1, X0       // Set TTBR0
    MSR TCR_EL1, X2         // Set TCR
    ISB                     // The ISB forces these changes to be seen before the MMU is enabled.
    
    MRS X0, SCTLR_EL1       // Read System Control Register configuration data
    ORR X0, X0, #1          // Set [M] bit and enable the MMU.
    MSR SCTLR_EL1, X0       // Write System Control Register configuration data
    ISB                     // The ISB forces these changes to be seen by the next instruction.

.globl jump_to_el1
jump_to_el1:
    LDR X0, main
    MSR ELR_EL3, X0 
    LDR X0, =0x03C08000
    MSR SP_EL1, X0          // Set SP
    MOV X0, #0x5
    MSR SPSR_EL3, X0
    ERET

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
