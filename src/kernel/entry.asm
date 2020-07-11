[BITS 64]
section .text
[GLOBAL _start]
[EXTERN _init]
[EXTERN kernel_entry]

_start:
    mov rsp, STACK_TOP
    call _init
    call kernel_entry

    hlt

section .bss
stack:
    resb 32768      ; 内核栈
STACK_TOP equ $-1