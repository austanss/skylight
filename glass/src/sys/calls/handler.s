global syscall_dispatch
syscall_dispatch:
    mov r10, rsp
    lea r11, [rel syscall_sp]
    mov rsp, [r11]
    mov rbp, rsp
    push rbp
    push r10

    xor r10, r10

    pop rsp
    pop rbp
    o64 sysret

section .bss

global syscall_sp
syscall_sp:
    resb 0x08