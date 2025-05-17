include Makefile.inc

KERNEL=kernel.bin
MEMORY_MANAGER ?= bitmap

# Common source files (used by both memory managers)
COMMON_SOURCES=$(wildcard *.c)
COMMON_SOURCES_ASM=$(wildcard asm/*.asm)
COMMON_OBJECTS=$(COMMON_SOURCES:.c=.o)
COMMON_OBJECTS_ASM=$(COMMON_SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

# Find all utility files except memory managers
UTILS_SOURCES=$(filter-out utils/bitmap.c utils/buddy.c, $(wildcard utils/*.c))
UTILS_OBJECTS=$(UTILS_SOURCES:.c=.o)

# Objects based on selected memory manager
ifeq ($(MEMORY_MANAGER),buddy)
	GCCFLAGS+=-DBUDDY
	MM_SOURCE=utils/buddy.c
else
	GCCFLAGS+=-DBITMAP
	MM_SOURCE=utils/bitmap.c
endif

MM_OBJECT=$(MM_SOURCE:.c=.o)
ALL_OBJECTS=$(COMMON_OBJECTS) $(UTILS_OBJECTS) $(MM_OBJECT) $(COMMON_OBJECTS_ASM)

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(ALL_OBJECTS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(ALL_OBJECTS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(ALL_OBJECTS) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o utils/*.o *.bin *.elf

.PHONY: all clean
