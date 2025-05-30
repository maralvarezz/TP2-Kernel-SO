include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

# Add library sources and objects
LIB_SOURCES=$(wildcard lib/*.c)
LIB_OBJECTS=$(LIB_SOURCES:lib/%.c=obj/%.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM) $(LIB_OBJECTS)
	$(GCC) $(GCCFLAGS) -I./include -I./lib -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(LIB_OBJECTS) $(SOURCES) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -I./include -I./lib -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(LIB_OBJECTS) $(SOURCES) -o ../0000-sampleCodeModule.elf

obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule to build lib objects
obj/%.o: lib/%.c
	mkdir -p obj
	$(GCC) $(GCCFLAGS) -I./include -I./lib -c $< -o $@

clean:
	rm -rf *.o
	rm -rf obj

.PHONY: all clean print
