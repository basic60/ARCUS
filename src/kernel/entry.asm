[BITS 64]
section .text
[GLOBAL _start]
[EXTERN _init]
[EXTERN kernel_entry]

_start:
    call _init
    call kernel_entry