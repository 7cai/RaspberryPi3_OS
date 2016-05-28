
.macro syscall_entry name id
    .globl \name
    \name:
        svc \id
        ret
.endm

    //.align 12

.globl syscall_vector
syscall_vector:
    syscall_entry printf, #0
    syscall_entry sleep, #1
