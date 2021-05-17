global boot
extern xboot

boot:
    cli
    
    xor rbp, rbp
    push rbp
    mov rbp, rsp
    
    call xboot
    jmp $