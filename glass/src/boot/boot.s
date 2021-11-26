extern gdt_assemble
extern idt_assemble
extern get_tag
extern pmm_start
extern paging_reload
extern tss_install
extern paging_edit_page
extern pmm_alloc_page
extern acpi_load_rsdp
extern apic_initialize
extern configure_math_extensions
extern install_syscalls
extern pci_conf_load_cache
extern paging_map_page
extern tty_render_glyph

global boot

section .text

boot:
    cli
    cld
    
    xor ebp, ebp

    push rdi

    call gdt_assemble

    call idt_assemble
    sti

    call configure_math_extensions

    xor eax, eax
    mov fs, ax
    mov gs, ax

    pop rdi

    lea rax, [rel bootctx]
    mov [rax], rdi

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

    xor edi, edi
    call tss_install

    lea rdi, [rel userspace]
    mov rsi, (0x001 | 0x002 | 0x004)
    call paging_edit_page

    xor edi, edi
    xor esi, esi
    call pmm_alloc_page
    mov rbx, rax
    mov rdi, rbx
    mov rsi, (0x001 | 0x002 | 0x004)
    call paging_edit_page
    add rbx, 0x1000

    pop rdi
    push rdi

    mov rsi, rdi
    mov rdx, 0xffffffff00000000
    not rdx
    and rsi, rdx
    mov rdx, (0x01 | 0x02)
    call paging_map_page

    pop rdi
    push rdi

    mov rsi, 0x9e1786930a375e78
    call get_tag
    lea rdi, [rax + (8*2)]
    mov rdi, [rdi]
    call acpi_load_rsdp
    
    mov rdi, rax
    call apic_initialize

    call pci_conf_load_cache

    call install_syscalls

    mov rdi, 24
    mov rsi, 24
    mov rdx, 'C'
    call tty_render_glyph

    pop rdi
    xor edi, edi
    xor esi, esi
    xor eax, eax


    jmp $

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

section .text

align 4096
userspace:
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx

    xor rdi, rdi
    xor rsi, rsi

    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    jmp $
    
section .bss
global bootctx
bootctx:
    resq 1