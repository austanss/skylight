global gdt_reload
gdt_reload:
    push rbp
    mov rbp, rsp
    mov ax, si
    lgdt [rdi]
    mov ds, ax
    mov es, ax
    pop rbp
    pop rax
    push dx
    push rax
    xor rax, rax
    retfq