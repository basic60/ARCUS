[GLOBAL idt_flush] 
idt_flush:
    push rax
    xor rax, rax
    mov rax, rdi
    lidt [rax]
    pop rax
    sti
    ret

%macro pushall 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15   
%endmacro

%macro popall 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli
    push qword %1
    jmp isr_common_stub
%endmacro

%macro ISR_NOERRCODE 1   ; 无错误码的中断，则默认push一个无用的占位数据，确保之后interrupt_stack的数据无需特殊处理
[GLOBAL isr%1]
isr%1:
    cli
    push qword 0
    push qword %1
    jmp isr_common_stub
%endmacro

%macro ISR_PIC 1   ; 无错误码的中断，则默认push一个无用的占位数据，确保之后interrupt_stack的数据无需特殊处理
[GLOBAL isr%1]
isr%1:
    cli
    push qword 0
    push qword %1
    jmp isr_pic_common_stub
%endmacro

[extern isr_dispatcher]
isr_common_stub:
    pushall

    xor rax,rax
    mov ax,ds
    push rax        ;保存数据段描述符

    mov ax, 0x10     ;加载内核数据段描述符
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rdi, rsp
    call isr_dispatcher
	pop rbx        ; 恢复原来的数据段描述符
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx

    add rsp, 16    ; 清理栈中的错误码和中断号
    popall
    sti
    iretq

[extern isr_pic_dispatcher]
isr_pic_common_stub:
    pushall

    xor rax,rax
    mov ax,ds
    push rax        ;保存数据段描述符

    mov ax, 0x10     ;加载内核数据段描述符
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rdi, rsp
    call isr_pic_dispatcher
	pop rbx        ; 恢复原来的数据段描述符
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx

    add rsp, 16    ; 清理栈中的错误码和中断号
    popall
    sti
    iretq


ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

ISR_PIC 32 	; 电脑系统计时器
ISR_PIC 33 	; 键盘
ISR_PIC 34 	; 与 IRQ9 相接，MPU-401 MD 使用
ISR_PIC 35 	; 串口设备
ISR_PIC 36 	; 串口设备
ISR_PIC 37 	; 建议声卡使用
ISR_PIC 38 	; 软驱传输控制使用
ISR_PIC 39 	; 打印机传输控制使用
ISR_PIC 40 	; 即时时钟
ISR_PIC 41 	; 与 IRQ2 相接，可设定给其他硬件
ISR_PIC 42 	; 建议网卡使用
ISR_PIC 43 	; 建议 AGP 显卡使用
ISR_PIC 44 	; 接 PS/2 鼠标，也可设定给其他硬件
ISR_PIC 45 	; 协处理器使用
ISR_PIC 46 	; IDE0 传输控制使用
ISR_PIC 47 	; IDE1 传输控制使用