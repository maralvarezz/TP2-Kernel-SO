# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g -I. -fno-builtin-malloc -fno-builtin-free

all: buddy bitmap prio processes sync

# Compilación de buddy
buddy:
    $(CC) $(CFLAGS) -DBUDDY testMain.c test_mm.c test_util.c buddy.c -o test_buddy syscall.c

# Compilación de bitmap
bitmap:
    $(CC) $(CFLAGS) -DBITMAP testMain.c test_mm.c test_util.c bitmap.c -o test_bitmap syscall.c

# Compilación de prio
prio:
    $(CC) $(CFLAGS) testMain.c test_prio.c test_util.c -o test_prio syscall.c

# Compilación de processes
processes:
    $(CC) $(CFLAGS) testMain.c test_processes.c test_util.c -o test_processes syscall.c

# Compilación de sync
sync:
    $(CC) $(CFLAGS) testMain.c test_sync.c test_util.c -o test_sync syscall.c

# Limpieza
clean:
    rm -f *.o test_buddy test_bitmap test_prio test_processes test_sync