INCLUDES	  := -I../include/ -I include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: elf.o syscall.o

syscall.o :
	$(AS) $(INCLUDES) syscall.s -o syscall.o

clean:
	rm -rf *~ *.o

include ../include.mk
