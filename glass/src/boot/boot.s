extern gdt_assemble
extern idt_assemble
extern get_tag
extern pmm_start
extern paging_reload
extern tss_install
extern paging_edit_page
extern pmm_alloc_page
extern acpi_load_rsdp
extern acpi_get_table
extern apic_initialize

global boot

section .text

boot:
    cli
    cld
    
    xor rbp, rbp

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

    mov rsi, 0x2187f79e8612de07
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
    push rdi

    mov rsi, 0x9e1786930a375e78
    call get_tag
    lea rdi, [rax + (8*2)]
    mov rdi, [rdi]
    call acpi_load_rsdp
    
    mov rdi, rax
    call apic_initialize

    pop rdi
    xor rdi, rdi
    xor rsi, rsi
    xor rax, rax

    mov ax, 0x1B
    mov ds, ax
    mov es, ax

    pop rbp

    sti

    push rax
    push rbx
    pushfq
    push 0x23
    lea rcx, [rel userspace]
    push rcx
    iretq

section .text

align 4096
userspace:
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    jmp $