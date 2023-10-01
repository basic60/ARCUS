[BITS 16]
org 7c00h
start:
    mov ax, cs
    mov ds, ax
    mov es, ax           
    mov esp, 7c00h
    jmp load_stage2

disk_rw_struct:
    db 16  ; size of disk_rw_struct, 10h
    db 0   ; reversed, must be 0
    dw 0   ; number of sectors
    dd 0   ; target address
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

; 第一个扇区代码作用：读取硬盘加载加下来几个扇区的代码到内存中
load_stage2:
    push dword 0x7e00   ; target address
    push word 50        ; number of blocks
    push dword 1        ; start LBA number
    call read_disk_by_int13h
    add esp, 10
    jmp enter_long_mode

times 510-($-$$) db 0
dw 0xaa55

E820_BUFFER equ 0xc000
PAGE_TABLE equ 0x40000
CODE_SEG equ 0x0008
DATA_SEG equ 0x0010

gdt64:
.Null:
    dq 0                              ; Null Descriptor - should be present.
.Code:
    dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
    dq 0x0000920000000000             ; 64-bit data descriptor (read/write).
ALIGN 4, dw 0
.pointer:
    dw $ - gdt64 - 1                    ; 16-bit Size (Limit) of GDT.
    dd gdt64

enter_long_mode:
    call memory_detect      ; 利用e820中断探测内存
    call fill_page_table    ; 初始化临时页表
    call enable_paging      ; 开启分页
    lgdt [gdt64.pointer]
    jmp CODE_SEG:long_mode_entry
    
mmap_entry_count equ E820_BUFFER
memory_detect:
    mov edi, 0xc004
    xor ebx, ebx		
	xor bp, bp		
	mov edx, 0x0534D4150
	mov eax, 0xe820
	mov [es:edi + 20], dword 1	
	mov ecx, 24
	int 0x15
    jc .failed
    mov edx, 0x0534D4150
	cmp eax, edx
	jne short .failed
	test ebx, ebx
	je .failed
	jmp .jmpin
    .e820_loop:
	    mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	    mov [es:edi + 20], dword 1	; force a valid ACPI 3.X entry
	    mov ecx, 24		; ask for 24 bytes again
	    int 0x15
	    jc short .detect_finish		; carry set means "end of list already reached"
	    mov edx, 0x0534D4150	; repair potentially trashed register
    .jmpin:
        jcxz .skip_entry		; skip any 0 length entries
        cmp cl, 20		; got a 24 byte ACPI 3.X response?
        jbe .add_idx
        test byte [es:edi + 20], 1	; if so: is the "ignore this data" bit clear?
        je .skip_entry
    .add_idx:
        mov ecx, dword [es:edi + 8]	; get lower uint32_t of memory region length
        or ecx, dword [es:edi + 12]	; "or" it with upper uint32_t to test for zero
        jz .skip_entry		; if length uint64_t is 0, skip entry
        inc bp			; got a good entry: ++count, move to next storage spot
        add edi, 24
    .skip_entry:
        test ebx, ebx		; if ebx resets to 0, list is complete
        jne .e820_loop
    .detect_finish:
        mov [mmap_entry_count], bp
        clc
        ret
    .failed:
        hlt

fill_page_table:
    mov edi, PAGE_TABLE   ; page talbe start at 0x40000, occupy 20KB memroy and map the first 26MB
    push edi
    mov ecx, 0x10000
    xor eax, eax
    cld
    rep stosd          ; zero out 64KB memory
    pop edi

    lea eax, [es:edi + 0x1000]     
    or eax, 3 
    mov [es:edi], eax

    lea eax, [es:edi + 0x2000]
	or eax, 3 
    mov [es:edi + 0x1000], eax

    mov ebx, 0x3000
    mov edx, 0x2000
    mov ecx, 52
    .loop_p4:
        lea eax, [es:edi + ebx]        
        or eax, 3
        mov [es:edi + edx], eax

        add ebx, 0x1000
        add edx, 8
        dec ecx
        cmp ecx, 0
        jne .loop_p4

    push edi               
    lea edi, [es:edi + 0x3000]
    mov eax, 3 
    .loop_page_table:
        mov [es:edi], eax
        add eax, 0x1000
        add edi, 8
        cmp eax, 0x1a00000       
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

times 1024 - ($-start) db 0

multi_core_boot:    ; 0x8000开始
    jmp 0x0000:ap_main
ap_main:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov eax, 0x1
    cpuid
    and ebx, 0xff000000
    shr ebx, 24
    mov eax, ebx                 ; 获取cpuid
    cmp eax, 8                   ; 最多支持8C
    jle .start_multi_core_boot
    hlt
    .start_multi_core_boot:
        mov ebx, 7c00h
        .minus_stack_loop:
            cmp eax, 0
            je .do_start_multi_core
            sub eax, 1
            sub ebx, 400h            ; 每个核分配不同的栈，初始1kb
            jmp .minus_stack_loop
        .do_start_multi_core:
            mov esp, ebx
            call enable_paging      ; 开启分页
            lgdt [gdt64.pointer]
            jmp CODE_SEG:long_mode_entry

[BITS 64]
long_mode_entry:
    mov ax,DATA_SEG
    mov ds,ax       ;DS存放数据段描述符。
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    call cli_clear
    jmp 0x8200

cli_clear:
    mov edi, 0xB8000
    mov ecx, 500
    mov eax, 0x0030       ; Set the value to set the screen to: Blue background, white foreground, blank spaces.
    rep stosq
    mov edi, 0xb8000         
    ret