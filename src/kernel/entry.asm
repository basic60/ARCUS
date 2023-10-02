[BITS 64]
section .text
[GLOBAL _start]
[EXTERN _init]
[EXTERN kernel_entry]

; rsi中的值为is_bsp , rdx中的值为cpuid
_start:
    call get_apic_id
    mov rdx, rax
    mov rax, STACK_TOP
    .minus_stack_loop:
        cmp rdx, 0
        je .jump_to_kernel
        sub rax, 8192
        sub rdx, 1
        jmp .minus_stack_loop
    .jump_to_kernel:
        mov rsp, rax
        call _init
        call kernel_entry
        hlt

get_apic_id:
    mov rax, 0x1
    cpuid
    and ebx, 0xff000000
    shr ebx, 24
    mov eax, ebx
    ret

section .bss
stack:
    resb 65536      ; 内核栈
STACK_TOP equ $-1