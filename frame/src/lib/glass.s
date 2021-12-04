global rdinfo
rdinfo:
    mov ax, 0x0000
    syscall
    ret

global pmap
pmap:
    mov ax, 0x0001
    syscall
    ret

global punmap
punmap:
    mov ax, 0x0002
    syscall
    ret
