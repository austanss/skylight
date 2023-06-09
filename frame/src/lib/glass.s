global rdinfo
rdinfo:
    mov eax, 0x0000
    syscall
    ret

global pmap
pmap:
    mov eax, 0x0001
    syscall
    ret

global punmap
punmap:
    mov eax, 0x0002
    syscall
    ret
