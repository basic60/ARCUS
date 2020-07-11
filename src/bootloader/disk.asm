[GLOBAL ld_read_disk]
[section .text]

ld_read_disk:
    push rbp
    mov rbp, rsp
    sub rsp, 24

    mov [rsp], rdi
    mov [rsp + 8], rsi
    mov [rsp + 16], rdx
    

    mov r8, [rsp]    ; lba number to start reading
    mov rbx, [rsp + 8]   ; the number of sectors to be read

    mov dx, 0x1f6
    mov al, 0x40|(0<<4)
    out dx, al       ; drive and some magic number 

    mov dx, 0x1f1
    mov al, 0x00
    out dx, al

    mov dx, 0x1f2
    mov al, bh
    out dx, al

    mov dx, 0x1f1
    mov al, 0x00
    out dx, al       ; send two null bytes to 0x1f1

    mov dx, 0x1f3
    mov rax, 0xffffffffffffffff
    and rax, r8
    shr rax, 24
    out dx, al       ; bit 24-31

    mov dx, 0x1f4
    mov rax, 0xffffffffffffffff
    and rax, r8
    shr rax, 32
    out dx, al       ; bit 32-39

    mov dx, 0x1f5
    mov rax, 0xffffffffffffffff
    and rax, r8
    shr rax, 40
    out dx, al       ; bit 40-47

    mov dx, 0x1f2
    mov al, bl
    out dx, al       ; 16-bit sector count

    mov dx, 0x1f3
    mov rax, 0xffffffffffffffff
    and rax, r8
    out dx, al       ; bit 0-7

    mov dx, 0x1f4
    mov rax, 0xffffffffffffffff
    and rax, r8
    shr rax, 8
    out dx, al       ; bit 8-15

    mov dx, 0x1f5
    mov rax, 0xffffffffffffffff
    and rax, r8
    shr rax, 16
    out dx, al       ; bit 16-23

    mov dx, 0x1f7
    mov al, 0x24
    out dx, al       ; send command 

.waits:
    in al, dx
    test al, 0x80  ; test BSY 
    jne .waits
    test al, 0x8   ; test DQR 
    je .waits     ; wait unil avilable (BSY bit is clear)

    mov rdi, [rsp + 16]
    mov dx, 0x1f0
.loop:
    mov rcx, 10000
.ready:
    mov dx, 0x1f7
    in al, dx
    test al, 0x8   ; test DQR 
    je .ready     ; wait unil avilable (BSY bit is clear)
    mov dx, 0x1f0

    mov cx, 256
    rep insw

    dec rbx
    cmp rbx, 0
    jnz .loop

    mov rsp, rbp
    pop rbp
    ret