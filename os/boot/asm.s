.globl _start
_start:
    b skip

.space 0x8000-0x4,0

skip:
    mrs x0,mpidr_el1
    mov x1,#0xC1000000
    bic x0,x0,x1
    cbz x0,master
    b hang

master:
    mov sp,#0x08000000
    bl bootloader

hang:
    b hang

.globl PUT32
PUT32:
    str w1,[x0]
    ret

.globl GET32
GET32:
    ldr w0,[x0]
    ret

.globl GETPC
GETPC:
    mov x0,x30
    ret

.globl BRANCHTO
BRANCHTO:
    mov w30,w0
    ret


.globl dummy
dummy:
    ret

