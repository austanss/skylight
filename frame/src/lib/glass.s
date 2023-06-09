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

global fb_req:
fb_req:
    mov eax, 0x0003
    syscall
    ret

global fb_kill:
fb_kill:
    mov eax, 0x0004
    syscall
    ret
