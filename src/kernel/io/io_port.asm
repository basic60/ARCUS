[GLOBAL read_io_port]
[GLOBAL write_io_port]

read_io_port:
    mov rdx,rdi
    in al, dx      
    ret

write_io_port:
    mov   rdx, rdi
    mov   rax, rsi
    out   dx, al  
    ret