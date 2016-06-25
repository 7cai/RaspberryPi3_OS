link_script   := loader

INCLUDES	  := -I../os/lib/include/ -I../os/include/ -I../os/user/include/

objects       := test.o			\
				 syscall.o		\
				 syscall.out

.PHONY: all clean

all: clean $(objects) user

user:
	$(LD) $(objects) -T $(link_script) -e umain -o user.elf
	./ElfFormatter user.elf

clean:
	rm -rf *~ *.o *.out *.elf

debug: all
	$(OBJDUMP) -D -m aarch64 user.elf > user.list

test.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c test.c

syscall.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c ../os/user/syscall.c -o syscall.o

syscall.out:
	$(AS) $(INCLUDES) ../os/user/syscall.s -o syscall.out

include ../os/include.mk
