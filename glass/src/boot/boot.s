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
    cli                 ; avoid exceptions or interruptions because idt is undefined
    cld                 ; standard c calling convention requirements
    
    xor ebp, ebp        ; shitty (needs fixing) attempt at a stack frame

    push rdi            ; stack the stivale2 pointer as rdi is never preserved

    call gdt_assemble   ; assemble the bonito gdt

    call idt_assemble   ; assemble the bonito idt
    sti                 ; yay, interrupts work now!

    call configure_math_extensions  ; floating points, sse, all those goodies

    xor eax, eax                    
    mov fs, ax                  ; zeroing (currently irrelevant) segment registers
    mov gs, ax

    pop rdi             ; retrieve stivale2 pointer

    call stivale2_reinterpret   ; reinterpret stivale2 information to match internal protocol (boot protocol abstraction)
    
    xor edi, edi        ; we don't need that mf no more

    call pmm_start      ; start the beautiful pmm

    call paging_reload  ; hope no page faults >>>>:((((((

    xor edi, edi        ; we're not passing anything
    call tss_install    ; instalar la tss bonita

    lea rdi, [rel userspace]
    mov rsi, (0x001 | 0x002 | 0x004)    ; mapping the kernel userspace stub page with user permissions
    call paging_edit_page

    xor edi, edi
    xor esi, esi
    call pmm_alloc_page
    mov rbx, rax
    mov rdi, rbx
    mov r12, rbx                        ; move the value to a calling convention preserved register
    mov rsi, (0x001 | 0x002 | 0x004)    ; allocate a comically small userspace stack, (TODO dynamically extended)
    call paging_edit_page
    add rbx, 0x1000                     ; this is a preserved register so dont worry

    call apic_initialize                ; initialize the Local APIC and IOAPIC

    call pci_conf_load_cache            ; load pci devices

    call install_syscalls               ; install (los tontos) system calls

    call local_timer_calibrate          ; calibrate the local APIC timer (using PIT)

    xor edi, edi
    xor esi, esi                ; cleanup scratch registers
    xor eax, eax

    mov rdi, frame_id           
    call get_boot_module        ; get the Frame executable
    mov rdi, [rax]              ; accessing first field of the returned structure
    call elf_load_program       ; exec is module in memory, this function loads the elf segments properly and ready for execution
    mov rdx, rax                ; save the return address as this is the entry point (in rdx cuz no more c functions)

    pop rbp         ; 0 from stivale2 return address, cleaning off the kernel stack
    ; did the stack frame ever matter? no

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
    ; this is a proper stack frame

    push rdx ; save this register, muy importante entry address

    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx

    xor rdi, rdi
    xor rsi, rsi
    ; no register data from the kernel should be preserved in any register
    ; that would lowkey be a security vulnerability
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

    call rdx    ; call Frame boyos! rdx register is scratch/parameter and will be overriden

    ; no safe handling?
    ; because if the init system reaches the `ret` here it deserves to #GP fault anyway
    ; maybe will fix when kernel panics implemented
    
section .data
frame_id:
    db "frame.se",0     ; null-terminated c-string
