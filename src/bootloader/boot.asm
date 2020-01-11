[BITS 16]
org 7c00h
mov si, 0
mov ax, cs
mov ds, ax
mov es, ax           
mov sp, 7c00h
jmp load_stage2

disk_rw_struct:
    db 16  ; size of disk_rw_struct, 10h
    db 0   ; reversed, must be 0
    dw 0   ; number of sectors
    dd 0   ; target addrr=ess
    dq 0   ; start LBA number

read_disk_by_int13h:
    mov eax, dword [esp + 8]
    mov dword [disk_rw_struct + 4], eax
    mov ax, [esp + 6]
    mov word [disk_rw_struct + 2], ax
    mov eax,dword [esp + 2]
    mov dword [disk_rw_struct + 8], eax
    mov ax, 4200h
    mov dx, 0080h
    mov si, disk_rw_struct
    int 13h
    ret

load_stage2:
    push dword 0x7e00   ; target address
    push word 50        ; number of blocks
    push dword 1        ; start LBA number
    call read_disk_by_int13h
    add esp,10
    jmp enter_long_mode

times 510-($-$$) db 0
dw 0xaa55

enter_long_mode:
    jmp $