GLOBAL write
GLOBAL read
GLOBAL getSeconds
GLOBAL clear
GLOBAL getInfoReg
GLOBAL setFontSize
GLOBAL getScreenResolution
GLOBAL drawRect
GLOBAL getTicks
GLOBAL getMemory
GLOBAL kaboom
GLOBAL setFontColor
GLOBAL getFontColor

read:
    mov rax, 0
    int 80h
    ret

write:
    mov rax, 1
    int 80h
    ret

clear:
    mov rax, 2
    int 80h
    ret

getSeconds:
    mov rax, 3
    int 80h
    ret

getInfoReg:
    mov rax, 4
    int 80h
    ret

setFontSize:
    mov rax, 5
    int 80h
    ret

getScreenResolution:
    mov rax, 6
    int 80h
    ret

drawRect:
    mov rax, 7
    mov r10, rcx
    int 80h
    ret

getTicks:
    mov rax, 8
    int 80h
    ret

getMemory:
    mov rax, 9
    int 80h
    ret

setFontColor:
    mov rax, 11
    int 80h
    ret

getFontColor:
    mov rax, 12
    int 80h
    ret

getMemInfo:
    mov rax, 13
    int 80h
    ret

exit:
    mov rax, 14
    int 80h
    ret

createProc:
    mov rax, 15
    int 80h
    ret

processInfo:
    mov rax, 16
    int 80h
    ret

getPid:
    mov rax, 17
    int 80h
    ret

killProc:
    mov rax, 18
    int 80h
    ret

changePrio:
    mov rax, 19
    int 80h
    ret

blockProc:
    mov rax, 20
    int 80h
    ret

unblockProc:
    mov rax, 21
    int 80h
    ret

yieldCPU:
    mov rax, 22
    int 80h
    ret

chauCPU:
    mov rax, 23
    int 80h
    ret

waitProcess:
    mov rax, 24
    int 80h
    ret

openPipe:
    mov rax, 25
    int 80h
    ret

closePipe:
    mov rax, 26
    int 80h
    ret

semCreate:
    mov rax, 27
    int 80h
    ret

semWait:
    mov rax, 28
    int 80h
    ret

semPost:
    mov rax, 29
    int 80h
    ret

semOpen:
    mov rax, 30
    int 80h
    ret

semClose:  
    mov rax, 31
    int 80h
    ret

allocMem:
    mov rax, 32
    int 80h
    ret

freeMem:
    mov rax, 33
    int 80h
    ret

