GLOBAL waitingProcess
GLOBAL liberateProcess

section .text

waitingProcess:
    mov al, 1
.loop:
    xchg al, [rdi]
    test al, al
    jz .loop
    ret

liberateProcess:
    mov al, 0
    mov [rdi], al
    ret

