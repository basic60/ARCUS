[GLOBAL change_page_table]

change_page_table:
    mov rax, rdi
    mov cr3, rax
    ret