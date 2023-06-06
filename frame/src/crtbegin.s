global _start
extern main

default rel

_start:
    mov rsp, rdi
    xor rbp, rbp
    push rbp
    mov rbp, rsp
    xor edi, edi
    mov bx, 0x0101
    call $+(main-$)
    ret
    