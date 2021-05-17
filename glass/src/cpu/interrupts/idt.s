global idt_reload
idt_reload:
    push rbp
    mov rbp, rsp
    pushfq
    cli
    lidt [rdi]
    sti
    popfq
    pop rbp
    ret