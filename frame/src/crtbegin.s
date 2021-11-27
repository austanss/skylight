global _start
extern main

default rel

_start:
    lea rax, [rel main]
    call rax
    ret
    