
INCLUDES	  := -I../lib/include/ -I../driver/include/ -I../mm/include/ -I../include/


%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: asm.o el2.o main.o

asm.o :
	$(AS) $(INCLUDES) asm.s -o asm.o

clean:
	rm -rf *~ *.o

include ../include.mk
