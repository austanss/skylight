global syscall_dispatch

%define nsyscalls 3

syscall_dispatch:
    push rcx        ; save rcx
    mov rcx, 0xC0000102 ; identifier of MSR
    mov r15, rax
    rdmsr           ; read the IA32_KERNEL_GS_BASE MSR
    pop rcx         ; restore rcx
    shr rdx, 0x20   ; read msr values
    or rax, rdx     ; combine the two registers
    mov r10, rsp    ; save rsp
    mov rsp, rax    ; set rsp to the kernel stack
    mov rbp, rsp    ; create stack frame
    push rbp        ; create stack frame
    push r10        ; save rsp

    cmp r15, nsyscalls  ; check if syscall number is valid
    jge $+(.generate_ud-$)    ; if not, generate #UD

    lea r10, [rel syscall_table]; relative address of syscall table
    lea r10, [r10 + (rax * 8)]  ; relative address of syscall
    mov rax, [r10]              ; load the syscall address

    xor r10d, r10d  ; clear r10

    cld             ; c calling convention cld
    call rax        ; call the syscall

    .sysret:
    pop rsp         ; restore rsp
    pop rbp         ; restore rbp
    o64 sysret      ; return to user mode

    .generate_ud:
    xor eax, eax    ; clear rax
    mov [rcx], rax  ; set null invalid opcode at return address to generate #UD
    lea r15, [rel .sysret]
    jmp r15     ; like destroys the executing program, but it's fine it shouldn't have made a bad syscall

section .data

extern rdinfo
extern pmap
extern punmap

syscall_table:
    dq rdinfo   ; read system information
    dq pmap     ; map a page
    dq punmap   ; unmap a page
    dq 0x0000000000000000   ; null syscall