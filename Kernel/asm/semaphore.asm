GLOBAL waitingProcess
GLOBAL liberateProcess

section .text

waitingProcess:
    mov al, 1
.loop:
    xchg [rdi], al
    test al, al
    jnz .loop
    ret

liberateProcess:
    mov al, 0
    mov [rdi], al
    ret

