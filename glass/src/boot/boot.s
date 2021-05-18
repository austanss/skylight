extern xboot
extern gdt_assemble
extern idt_assemble
extern get_tag
extern pmm_start

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
    push rdi
    
    mov rsi, 0x2187f79e8612de07
    call get_tag
    mov rdi, rax
    call pmm_start

    pop rdi

    sti

h:  hlt
    jmp h