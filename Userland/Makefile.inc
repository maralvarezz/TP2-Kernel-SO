GCC=gcc
GPP=g++
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-g -m64 -fno-pie -no-pie -fno-exceptions -std=c99 -Wall -ffreestanding -nostdlib -fno-common -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc 
ARFLAGS=rvs
ASMFLAGS=-g -felf64
