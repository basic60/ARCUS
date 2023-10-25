[GLOBAL get_apic_id]
[GLOBAL start_multi_core]
[EXTERN sleep]
[section .text]
[BITS 64]
APIC_ICR_LOW equ 0xFEE00300

get_apic_id:
    mov rax, 0x1
    cpuid
    and ebx, 0xff000000
    shr ebx, 24
    mov eax, ebx
    ret

start_multi_core:
    ; 向所有其他 AP 发送 INIT
    mov eax, 000C4500H  ; Load ICR encoding for broadcast INIT IPI to all APs into EAX.
    mov esi, APIC_ICR_LOW
    mov [esi], eax

    push rdi
    mov rdi, 10
    call sleep      ; 10-millisecond sleep
    ; 向所有其他 AP 发送 SIPI
    mov eax, 0x000C4608
    mov [esi], eax

    mov rdi, 200
    call sleep      ; 200-microsecond delay loop
    pop rdi
    mov [esi], eax
    ret