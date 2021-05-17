extern xboot
extern gdt_assemble
extern idt_assemble

global boot
boot:
    cli
    cld
    
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    call gdt_assemble

    call idt_assemble

    call xboot
    jmp $