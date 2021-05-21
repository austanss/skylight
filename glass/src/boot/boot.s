extern xboot
extern gdt_assemble
extern idt_assemble
extern get_tag
extern pmm_start
extern paging_reload
extern tss_install

global boot

; Configure the environment, set up the CPU.
; Reload the GDT, assemble an IDT, remake the
; page tables, start the PMM, and load the TSS.
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
    push rdi

    mov rsi, 0x506461d2950408fa
    call get_tag
    mov rdi, rax
    call paging_reload

    xor rdi, rdi
    call tss_install

    pop rdi

    sti

h:  hlt
    jmp h