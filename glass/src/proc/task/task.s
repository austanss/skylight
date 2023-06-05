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
    ret

global _load_task_page_tables
_load_task_page_tables:
    push rbp
    mov rbp, rsp
    extern paging_walk_page
    call paging_walk_page ; rdi is virt; rax returns phys
    mov cr3, rax
    pop rbp
    ret

global _finalize_task_switch
_finalize_task_switch:
    cli
    push rbp
    mov rbp, rsp
    pop rbp     ; break c compliance for nitty gritty task switch
    ; make sure user stack is clean upon entry if we use it

    mov rsp, [rdi + 0x70]
    mov rbp, [rdi + 0x78]
    mov r15, [rdi + 0x80]
    push rsp            ; push user stack
    sti
    pushfq              ; push flags
    cli
    push 0x23           ; user code segment
    push r15            ; push user instruction pointer
    call $+(_load_task_general_registers-$)
    .end:
    iretq
