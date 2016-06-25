
.macro vector handler
    .align 7
    b \handler
.endm

.align 11
.globl vectors
vectors:
    vector SYNStub
    vector IRQStub
    vector SYNStub
    vector SYNStub

    vector SYNStub
    vector IRQStub
    vector SYNStub
    vector SYNStub

    vector SYNStub
    vector IRQStub
    vector SYNStub
    vector SYNStub

    vector SYNStub
    vector IRQStub
    vector SYNStub
    vector SYNStub

.macro push_time_stack
    str x0, [sp, #-16]!
    ldr x0, =0xffffff0001751000             // TIMESTACKTOP

    stp x29, x30, [x0, #-16]!
    stp x27, x28, [x0, #-16]!
    stp x25, x26, [x0, #-16]!
    stp x23, x24, [x0, #-16]!
    stp x21, x22, [x0, #-16]!
    stp x19, x20, [x0, #-16]!
    stp x17, x18, [x0, #-16]!
    stp x15, x16, [x0, #-16]!
    stp x13, x14, [x0, #-16]!
    stp x11, x12, [x0, #-16]!
    stp x9, x10, [x0, #-16]!
    stp x7, x8, [x0, #-16]!
    stp x5, x6, [x0, #-16]!
    stp x3, x4, [x0, #-16]!
    stp x1, x2, [x0, #-16]!

    mov x21, x0
    ldr x0, [sp], #16
    mrs x20, sp_el0
    stp x20, x0, [x21, #-16]!

    mrs x19, esr_el1
    mrs x20, elr_el1
    stp x20, x19, [x21, #-16]!

    mrs x19, spsr_el1
    str x19, [x21, #-8]!
.endm

.macro pop_time_stack
    ldr x30, =0xffffff0001750ee8            // TIMESTACK

    ldr x0, [x30], #8
    msr spsr_el1, x0

    ldp x0, x1, [x30], #16
    msr elr_el1, x0

    ldp x1, x0, [x30], #16
    msr sp_el0, x1

    ldp x1, x2, [x30], #16
    ldp x3, x4, [x30], #16
    ldp x5, x6, [x30], #16
    ldp x7, x8, [x30], #16
    ldp x9, x10, [x30], #16
    ldp x11, x12, [x30], #16
    ldp x13, x14, [x30], #16
    ldp x15, x16, [x30], #16
    ldp x17, x18, [x30], #16
    ldp x19, x20, [x30], #16
    ldp x21, x22, [x30], #16
    ldp x23, x24, [x30], #16
    ldp x25, x26, [x30], #16
    ldp x27, x28, [x30], #16
    ldp x29, x30, [x30], #0
.endm

.globl IRQStub
IRQStub:
    push_time_stack
    ldr x19, =0xFFFFFF0001000000// kernel stack
    mov sp, x19
    bl irq_handler

    pop_time_stack

    eret


.macro function_entry name id iss retaddr
    cmp \iss, \id
    b.ne 1f
    bl \name
    bl \retaddr
1:
.endm

.globl SYNStub
SYNStub:
    push_time_stack

    b synchronous_handler

stub_end:
    pop_time_stack

    eret

.globl synchronous_handler
synchronous_handler:
    mrs x19, esr_el1
    lsr x19, x19, #26
    
    cmp x19, #0x24
    b.eq data_abort_handler
    cmp x19, #0x15
    b.eq svc_handler
    b generic_handler

.globl data_abort_handler
data_abort_handler:
    ldr x19, =0xFFFFFF0001000000// kernel stack
    mov sp, x19
    str x30, [sp, #-8]!         // save x30 at first
    
    bl mmu_fault_handler
    
    ldr x30, [sp], #8
    b stub_end

.globl svc_handler
svc_handler:
    mrs x20, sp_el0
    mov sp, x20                 // change stack

    mrs x20, esr_el1
    bic x20, x20, #0xFE000000   // set iss

function_entrys:
    function_entry _putchar, #0, x20, stub_end
    function_entry _printf, #1, x20, stub_end
    function_entry _sleep, #2, x20, stub_end
    function_entry _getenvid, #3, x20, stub_end
    function_entry _ipc_can_recv, #4, x20, stub_end
    function_entry _ipc_can_send, #5, x20, stub_end
    ldr x19, =0xFFFFFF0001000000// kernel stack
    mov sp, x19
    function_entry _env_destroy, #6, x20, stub_end
    function_entry _fork, #7, x20, stub_end
    function_entry _sched_yield, #8, x20, stub_end
    bl generic_handler
