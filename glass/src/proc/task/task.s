_load_task_general_registers:
    mov rax, [rdi + 0x00]
    mov rbx, [rdi + 0x08]
    mov rcx, [rdi + 0x10]
    mov rdx, [rdi + 0x18]
    mov rsi, [rdi + 0x28]
    mov r8, [rdi + 0x30]
    mov r9, [rdi + 0x38]
    mov r10, [rdi + 0x40]
    mov r11, [rdi + 0x48]
    mov r12, [rdi + 0x50]
    mov r13, [rdi + 0x58]
    mov r14, [rdi + 0x60]
    mov rdi, [rdi + 0x20]
    jmp r15

global _load_task_page_tables
_load_task_page_tables:
    push rbp
    mov rbp, rsp
    mov rax, rdi
    mov cr3, rax
    pop rbp
    ret

global _finalize_task_switch
_finalize_task_switch:
    push rbp
    mov rbp, rsp
    mov rax, rdi
    pop rbp     ; break c compliance for nitty gritty task switch
    ; make sure user stack is clean upon entry if we use it

    mov rsp, [rax + 0x70]
    mov rbp, [rax + 0x78]
    mov r15, [rax + 0x80]
    lea rax, [rel _load_task_general_registers]
    jmp rax                

align 4096
_user_go:
    swapgs
    jmp r15
    ; issues are that missing userspace transition
    ; and preserving registers
    
