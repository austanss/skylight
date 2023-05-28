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

    call stivale2_reinterpret
    
    xor edi, edi

    call pmm_start

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

    mov rsi, rdi
    mov rdx, 0xffffffff00000000
    not rdx
    and rsi, rdx
    mov rdx, (0x01 | 0x02)
    call paging_map_page

    call apic_initialize

    call pci_conf_load_cache

    call install_syscalls

    call local_timer_calibrate

    xor edi, edi
    xor esi, esi
    xor eax, eax

    mov rdi, frame_id
    call get_boot_module
    mov rdi, [rax]
    call elf_load_program
    mov rdx, rax

    pop rbp         ; 0 from stivale2 return address

    lea rcx, [rel userspace]    ; rel loading userspace stub

    xor rax, rax
    mov ax, 0x1B
    mov ds, ax
    mov es, ax      ; loading userspace segments

    push rax    ; user data/stack segment
    push rbx    ; stack pointer
    pushfq      ; flags
    push 0x23   ; user code segment
    push rcx    ; creating false interrupt frame, return address
    iretq       ; mystical fake iretq manuever

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
