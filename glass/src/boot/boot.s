extern gdt_assemble
extern idt_assemble
extern get_tag
extern pmm_start
extern paging_reload
extern tss_install
extern paging_edit_page
extern pmm_alloc_page

global boot

; Configure the environment, set up the CPU.
; Reload the GDT, assemble an IDT, remake the
; page tables, start the PMM, and load the TSS.

; Then, maneuver into userspace ring 3 w/ iretq.
boot:
    cli
    cld
    
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    push rdi

    call gdt_assemble

    call idt_assemble

    xor rax, rax
    mov fs, ax
    mov gs, ax

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

    lea rdi, [rel userspace]
    mov rsi, (0x001 | 0x002 | 0x004)
    call paging_edit_page

    xor rdi, rdi
    xor rsi, rsi
    call pmm_alloc_page
    mov rbx, rax
    mov rdi, rbx
    mov rsi, (0x001 | 0x002 | 0x004)
    call paging_edit_page
    add rbx, 0x1000

    pop rdi
    xor rdi, rdi
    xor rsi, rsi
    xor rax, rax

    mov ax, 0x1B
    mov ds, ax
    mov es, ax

    pop rbp

    push rax
    push rbx
    pushfq
    push 0x23
    lea rcx, [rel userspace]
    push rcx
    iretq

align 4096
userspace:
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    jmp $