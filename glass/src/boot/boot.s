extern gdt_assemble
extern idt_assemble
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
extern tty_enable
extern tty_disable
extern elf_load_program
extern get_boot_module
extern local_timer_calibrate
extern stivale2_reinterpret

global _start64

section .text

_start64:
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
    push rdi

    call stivale2_reinterpret

    call pmm_start

    pop rdi
    push rdi
    call paging_reload

    call tty_enable

    cli
    hlt

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

    call apic_initialize

    call pci_conf_load_cache

    call install_syscalls

    call local_timer_calibrate

    pop rdi
    xor edi, edi
    xor esi, esi
    xor eax, eax

    call tty_disable

    mov rdi, frame_id
    call get_boot_module
    mov rdi, [rax]
    call elf_load_program
    mov rdx, rax

    lea rcx, [rel userspace]

    mov ax, 0x1B
    mov ds, ax
    mov es, ax

    pop rbp

    push rax
    push rbx
    pushfq
    push 0x23
    push rcx
    iretq

align 4096
userspace:
    xor rbp, rbp
    push rbp
    mov rbp, rsp

    push rdx

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

    pop rdx

    mov rdi, rsp

    call rdx
    
section .data
frame_id:
    db "frame.se",0

section .bss
