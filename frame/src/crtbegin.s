global _start
extern main

default rel

_start:
    mov rsp, rdi
    xor rbp, rbp
    push rbp
    mov rbp, rsp
    xor edi, edi
    lea rax, [rel main]
    call rax
    ret
    