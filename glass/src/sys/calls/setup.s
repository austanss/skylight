extern syscall_dispatch
extern tss_descriptors
extern syscall_sp

global install_syscalls
install_syscalls:
    lea rax, [rel syscall_dispatch]
    mov rdx, rax
    shr rdx, 0x20
    mov eax, eax
    mov edx, edx

    mov rcx, 0xc0000082
    wrmsr

    mov rcx, 0xc0000080
    rdmsr
    or eax, 0x01
    wrmsr
    mov rcx, 0xc0000081
    rdmsr
    mov edx, 0x00100008
    wrmsr

    xor eax, eax
    lea rax, [rel tss_descriptors]
    lea rsi, [rax + 0x04]
    lea rdi, [rel syscall_sp]
    movsq

    cli
    hlt

    ret