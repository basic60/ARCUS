[GLOBAL get_apic_id]
[section .text]

get_apic_id:
    mov rax, 0x1
    cpuid
    and ebx, 0xff000000
    shr ebx, 24
    mov eax, ebx
    ret