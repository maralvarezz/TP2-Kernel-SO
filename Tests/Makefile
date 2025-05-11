# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g -I. -fno-builtin-malloc -fno-builtin-free

# Compilación de buddy
buddy:
	$(CC) $(CFLAGS) -DBUDDY testMain.c test_mm.c test_util.c buddy.c -o test_buddy syscall.c

# Compilación de bitmap
bitmap:
	$(CC) $(CFLAGS) -DBITMAP testMain.c test_mm.c test_util.c bitmap.c -o test_bitmap syscall.c

# Limpieza
clean:
	rm -f *.o test_buddy test_bitmap
