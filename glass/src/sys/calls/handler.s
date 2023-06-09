global syscall_dispatch

%define nsyscalls 3

syscall_switch_kernel:
    push rbp
    mov rbp, rsp

    mov rax, [gs:0x20]
    mov [rax+0x00], rax ; get address of ctx and save rax

    mov rax, [gs:0x20]
    mov [rax+0x08], rbx
    mov [rax+0x10], rcx
    mov [rax+0x18], rdx
    mov [rax+0x20], rdi 
    mov [rax+0x28], rsi
    mov [rax+0x30], r8
    mov [rax+0x38], r9
    mov [rax+0x40], r10
    mov [rax+0x48], r11
    mov [rax+0x50], r12
    mov [rax+0x58], r13
    mov [rax+0x60], r14
    mov [rax+0x68], r15

    pop rbp
    ret

syscall_switch_user:
    push rbp
    mov rbp, rsp

    pop rbp
    ret

syscall_dispatch:
    swapgs          ; always coming from user always swapgs
    mov [gs:0x28], rsp ; save rsp
    mov rsp, [gs:0x10] ; load rsp
    push rbp        ; save rbp

    cmp rax, nsyscalls  ; check if syscall number is valid
    jge $+(.generate_ud-$)    ; if not, generate #UD

    lea r10, [rel syscall_table]; relative address of syscall table
    lea r10, [r10 + (rax * 8)]  ; relative address of syscall
    mov rax, [r10]              ; load the syscall address

    xor r10d, r10d  ; clear r10

    cld             ; c calling convention cld
    call rax        ; call the syscall

    .sysret:
    pop rbp         ; restore rbp
    mov rsp, [gs:0x28]  ; restore rsp
    swapgs
    o64 sysret      ; return to user mode

    .generate_ud:
    xor eax, eax    ; clear rax
    mov [rcx], rax  ; set null invalid opcode at return address to generate #UD
    jmp $+(.sysret-$)     ; like destroys the executing program, but it's fine it shouldn't have made a bad syscall

section .data

extern rdinfo
extern pmap
extern punmap

syscall_table:
    dq rdinfo   ; read system information
    dq pmap     ; map a page
    dq punmap   ; unmap a page
    dq 0x0000000000000000   ; null syscall