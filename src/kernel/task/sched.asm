[GLOBAL switch_task]

switch_task:
        ; 保存当前线程上下文
        push r8
        mov r8, rdi

        mov [r8 + 0],  r15              ;rsp包含的是调用switchKThread后压入的返回地址
        mov [r8 + 8],  r14
        mov [r8 + 16], r13
        mov [r8 + 24], r12
        mov [r8 + 32], r11
        mov [r8 + 40], r10
        mov [r8 + 48], r9
        
        mov r9, [rsp + 8]
        mov [r8 + 56], r9
        pop r8
        
        mov [r8 + 64], rdi
        mov [r8 + 72], rsi
        mov [r8 + 80], rbp
        mov [r8 + 88], rdx
        mov [r8 + 96], rcx
        mov [r8 + 104], rbx
        
        mov [r8 + 112], rax
        pushf
        pop r9
        mov[r8 + 120], r9
        mov[r8 + 128], rsp

        ; 切换到新线程的上下文
        mov r8, rsi
        mov r15, [r8 + 0]
        mov r14, [r8 + 8]
        mov r13, [r8 + 16]
        mov r12, [r8 + 24]
        mov r11, [r8 + 32]
        mov r10, [r8 + 40]

        mov r9, [r8 + 120]
        push r9
        popf

        mov r9, [r8 + 48]

        mov rdi, [r8 + 64]
        mov rsi, [r8 + 72] 
        mov rbp, [r8 + 80] 
        mov rdx, [r8 + 88] 
        mov rcx, [r8 + 96] 
        mov rbx, [r8 + 104]
        mov rax, [r8 + 112]
        mov rsp, [r8 + 128]
        mov r8, [r8 + 56]
        ret