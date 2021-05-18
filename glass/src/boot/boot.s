extern xboot
extern gdt_assemble
extern idt_assemble
extern get_tag

global boot

; Configure the environment, set up the CPU.
; Reload the GDT, assemble an IDT, remake the
; page tables, and start the PMM.
boot:
    cli
    cld
    
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    push rdi

    call gdt_assemble

    call idt_assemble

    pop rdi

    sti

h:  hlt
    jmp h