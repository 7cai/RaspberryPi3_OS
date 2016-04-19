
INCLUDES	  := -I./include/ -I../lib/include/ -I../env/include/ -I../include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: pmap.o tlb_asm.o

tlb_asm.o :
	$(AS) tlb_asm.S -o tlb_asm.o

clean:
	rm -rf *~ *.o


include ../include.mk
