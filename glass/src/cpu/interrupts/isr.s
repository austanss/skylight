
global isr_save_task_context
isr_save_task_context:
    push rbp
    mov rbp, rsp
    ; TODO proper full context switch
    mov rax, cr3
    mov rbx, [gs:0x20]
    mov [rbx+0x88], rax ; save task cr3
    extern paging_sync_cr3
    lea rax, [rel paging_sync_cr3]
    call rax
    mov rax, [gs:0x20]
    pop rbp
    ret

global isr_restore_task_context
isr_restore_task_context:
    push rbp
    mov rbp, rsp
    ; TODO proper full context switch
    mov rax, [gs:0x20]
    mov rax, [rax+0x88] ; load task cr3
    mov cr3, rax
    pop rbp
    ret

%macro isr_err_stub 1
isr_stub_%+%1:
    push %1
    jmp $+(isr_xframe_assembler-$)
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0
    push %1
    jmp $+(isr_xframe_assembler-$)
%endmacro

extern __routine_handlers

%macro irq_stub 1
irq_stub_%+%1:
    swapgs
    push r15
    mov r15, [gs:0x20]
    cmp r15, 0x00
    ; todo
    
    .yes_task:
    call isr_save_task_context
    mov [gs:0x20], rax  ; save task context returned in rax
    lea r15, [rel __routine_handlers]
    mov r15, [r15 + %1 * 8]
    call r15
    pop r15
    call isr_restore_task_context
    swapgs
    iretq
    
    .no_task:   ; interrupting kernel
    pop r15
    mov rbp, rsp
    push rbp
    lea r15, [rel __routine_handlers]
    mov r15, [r15 + %1 * 8]
    call r15
    pop rbp
    swapgs
    iretq

%endmacro

%macro pushagrd 0
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
%endmacro

%macro popagrd 0
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
%endmacro

%macro pushacrd 0
mov rax, cr0
push rax
mov rax, cr2
push rax
mov rax, cr3
push rax
mov rax, cr4
push rax
%endmacro

%macro popacrd 0
pop rax
mov cr4, rax
pop rax
mov cr3, rax
pop rax
mov cr2, rax
pop rax
mov cr0, rax
%endmacro

isr_xframe_assembler:
    push rbp
    mov rbp, rsp
    pushagrd
    pushacrd
    mov ax, ds
    push rax
    push qword 0
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    swapgs

    call isr_save_task_context

    lea rdi, [rsp + 0x10]
    extern isr_exception_handler
    call $+(isr_exception_handler-$)

    mov rdi, [gs:0x20]
    mov rax, [rdi + 0x88] ; task cr3
    mov cr3, rax

    call isr_restore_task_context

    pop rax
    pop rax
    mov ds, ax
    mov es, ax
    popacrd
    popagrd
    pop rbp
    add rsp, 0x10
    swapgs
    iretq

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
irq_stub        32
irq_stub        33
irq_stub        34
irq_stub        35
irq_stub        36
irq_stub        37
irq_stub        38

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dq isr_stub_%+i
%assign i i+1 
%endrep
%assign i 32
%rep    7
    dq irq_stub_%+i
%assign i i+1
%endrep
