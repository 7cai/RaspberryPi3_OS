
INCLUDES	  := -I../lib/include/ -I../driver/include/ -I../mm/include/ -I../include/


%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: asm.o main.o init.o

asm.o :
	$(AS) $(INCLUDES) asm.s -o asm.o

init.o :
	$(AS) $(INCLUDES) init.s -o init.o

clean:
	rm -rf *~ *.o

include ../include.mk
