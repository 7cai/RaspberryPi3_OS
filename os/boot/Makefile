
INCLUDES	  := -I../lib/include/ -I../driver/include/


%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: asm.o bootloader.o init.o

asm.o :
	$(AS) asm.s -o asm.o

clean:
	rm -rf *~ *.o

include ../include.mk
