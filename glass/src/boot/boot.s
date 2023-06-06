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
    call $+(gdt_assemble-$)   ; assemble the bonito gdt

    extern idt_assemble
    call $+(idt_assemble-$)   ; assemble the bonito idt
    sti                 ; yay, interrupts work now!

    extern configure_math_extensions
    call $+(configure_math_extensions-$)  ; floating points, sse, all those goodies

    xor eax, eax                    
    mov fs, ax                  ; zeroing (currently irrelevant) segment registers
    mov gs, ax

    pop rdi             ; retrieve stivale2 pointer

    extern stivale2_reinterpret
    call $+(stivale2_reinterpret-$)   ; reinterpret stivale2 information to match internal protocol (boot protocol abstraction)
    
    xor edi, edi        ; we don't need that mf no more

    extern pmm_start
    call $+(pmm_start-$)      ; start the beautiful pmm

    extern paging_reload_kernel_map
    call $+(paging_reload_kernel_map-$)   ; hope no page faults >>>>:((((((

    xor edi, edi        ; we're not passing anything
    extern tss_install
    call $+(tss_install-$)    ; instalar la tss bonita

    extern apic_initialize
    call $+(apic_initialize-$)                ; initialize the Local APIC and IOAPIC

    extern pci_conf_load_cache
    call $+(pci_conf_load_cache-$)            ; load pci devices

    extern install_syscalls
    call $+(install_syscalls-$)               ; install (los tontos) system calls

    extern local_timer_calibrate
    call $+(local_timer_calibrate-$)          ; calibrate the local APIC timer (using PIT)

    xor edi, edi
    xor esi, esi                ; cleanup scratch registers
    xor eax, eax

    mov rdi, $+(frame_id-$)           
    extern get_boot_module
    call $+(get_boot_module-$)        ; get the Frame executable
    mov rdi, [rax]              ; accessing first field of the returned structure (virt)
    extern elf_load_program
    call $+(elf_load_program-$)       ; exec is module in memory, this function loads the elf segments properly and ready for execution

    extern task_create_new
    mov rdi, rax                ; create the new task with the entry point
    call $+(task_create_new-$)      ; this function returns the task id

    extern task_select          ; select the task to run
    mov rdi, rax

    pop rbp
    pop rbp

    call $+(task_select-$)            ; select the task to run

    extern reboot
    jmp $+(reboot-$)                      ; reboot the system if the task_select function returns (which it shouldn't)
    
    
section .data
frame_id:
    db "frame.se",0     ; null-terminated c-string
