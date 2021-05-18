global memset
memset:
    push rbp
    mov rbp, rsp
    pushfq
    mov rdi, rdi
    mov rax, rsi
    push rcx
    mov rcx, rdx
    rep stosb
    pop rcx
    popfq
    pop rbp
    ret