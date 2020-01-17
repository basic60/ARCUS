[BITS 16]
org 7c00h
mov si, 0
mov ax, cs
mov ds, ax
mov es, ax           
mov esp, 7b000h
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
    add esp, 10
    jmp enter_long_mode

times 510-($-$$) db 0
dw 0xaa55

%define PAGE_TABLE 0x7b000
%define CODE_SEG   0x0008
%define DATA_SEG    0x0010

gdt64:
.Null:
    dq 0                              ; Null Descriptor - should be present.
.Code:
    dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
    dq 0x0000920000000000             ; 64-bit data descriptor (read/write).
.pointer:
    dw $ - gdt64 - 1                    ; 16-bit Size (Limit) of GDT.
    dd gdt64

enter_long_mode:
    call fill_page_table
    call enable_paging
    lgdt [gdt64.pointer]
    jmp CODE_SEG:long_mode_entry
    jmp $

fill_page_table:
    mov edi, PAGE_TABLE   ; page talbe start at 0x7b000, occupy 20KB memroy and map the first 4MB
    push edi
    mov ecx, 0x1400
    xor eax, eax
    cld
    rep stosd          ; zero out 20KB memory
    pop edi

    lea eax, [es:edi + 0x1000]     
    or eax, 3 
    mov [es:edi], eax

    lea eax, [es:edi + 0x2000]
	or eax, 3 
    mov [es:edi + 0x1000], eax

    lea eax, [es:edi + 0x3000]        
	or eax, 3
    mov [es:edi + 0x2000], eax
    
    lea eax, [es:edi + 0x4000]        
	or eax, 3
    mov [es:edi + 0x2008], eax

    push edi               
    lea edi, [es:edi + 0x3000]
    mov eax, 3 
    .loop_page_table:
        mov [es:edi], eax
        add eax, 0x1000
        add edi, 8
        cmp eax, 0x400000       
        jb .loop_page_table
    pop edi

    mov ax, 0x2401
    int 0x15              ; Enalbe A20 address line. 
    cli
    ret

enable_paging:
    ; enable pae and pge
    mov eax, 10100000b
    mov cr4, eax

    mov eax, PAGE_TABLE
    mov cr3, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100 
    wrmsr

    ; enable paging and protection
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    ret

[BITS 64]
long_mode_entry:
    mov ax, DATA_SEG
    mov ds, ax       ;DS存放数据段描述符。
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp $
