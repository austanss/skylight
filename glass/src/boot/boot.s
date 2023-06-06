global _start64

section .text
default rel

_start64:
    cli                 ; avoid exceptions or interruptions because idt is undefined
    cld                 ; standard c calling convention requirements
    
    xor ebp, ebp        ; shitty (needs fixing) attempt at a stack frame
    push rbp
    mov rbp, rsp
    push rdi            ; stack the stivale2 pointer as rdi is never preserved

    extern gdt_assemble
    lea r15, [rel gdt_assemble]
    call r15   ; assemble the bonito gdt

    extern idt_assemble
    lea r15, [rel idt_assemble]
    call r15   ; assemble the bonito idt
    sti                 ; yay, interrupts work now!

    extern configure_math_extensions
    lea r15, [rel configure_math_extensions]
    call r15  ; floating points, sse, all those goodies

    xor eax, eax                    
    mov fs, ax                  ; zeroing (currently irrelevant) segment registers
    mov gs, ax

    pop rdi             ; retrieve stivale2 pointer

    extern stivale2_reinterpret
    lea r15, [rel stivale2_reinterpret]
    call r15   ; reinterpret stivale2 information to match internal protocol (boot protocol abstraction)
    
    xor edi, edi        ; we don't need that mf no more

    extern pmm_start
    lea r15, [rel pmm_start]
    call r15      ; start the beautiful pmm

    cli
    hlt

    extern paging_reload_kernel_map
    lea r15, [rel paging_reload_kernel_map]
    call r15   ; hope no page faults >>>>:((((((

    xor edi, edi        ; we're not passing anything
    extern tss_install
    lea r15, [rel tss_install]
    call r15    ; instalar la tss bonita

    extern apic_initialize
    lea r15, [rel apic_initialize]
    call r15                ; initialize the Local APIC and IOAPIC

    extern pci_conf_load_cache
    lea r15, [rel pci_conf_load_cache]
    call r15            ; load pci devices

    extern install_syscalls
    lea r15, [rel install_syscalls]
    call r15               ; install (los tontos) system calls

    extern local_timer_calibrate
    lea r15, [rel local_timer_calibrate]
    call r15          ; calibrate the local APIC timer (using PIT)

    xor edi, edi
    xor esi, esi                ; cleanup scratch registers
    xor eax, eax

    mov rdi, $+(frame_id-$)           
    extern get_boot_module
    lea r15, [rel get_boot_module]  
    call r15        ; get the Frame executable
    mov rdi, [rax]              ; accessing first field of the returned structure (virt)
    extern elf_load_program
    lea r15, [rel elf_load_program]
    call r15       ; exec is module in memory, this function loads the elf segments properly and ready for execution

    extern task_create_new
    mov rdi, rax                ; create the new task with the entry point
    lea r15, [rel task_create_new]
    call r15

    extern task_select          ; select the task to run
    mov rdi, rax

    pop rbp
    pop rbp

    lea r15, [rel task_select]
    call r15            ; select the task to run

    extern reboot
    lea r15, [rel reboot]
    jmp r15                      ; reboot the system if the task_select function returns (which it shouldn't)
    
    
section .data
frame_id:
    db "frame.se",0     ; null-terminated c-string
