.macro syscall_entry name id
    .globl \name
    \name:
        svc \id
        ret
.endm


.globl syscall_vector
syscall_vector:
    syscall_entry putchar, #0
    syscall_entry printf, #1
    syscall_entry sleep, #2
    syscall_entry getenvid, #3
    syscall_entry ipc_can_recv, #4
    syscall_entry ipc_can_send, #5
    syscall_entry env_destroy, #6
    syscall_entry fork, #7
    syscall_entry yield, #8
