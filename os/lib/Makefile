
INCLUDES	  := -I../include/ -I include/ -I../mm/include/ -I../driver/include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: handler.o env.o sched.o print.o sleep.o kernel_elfloader.o handler_asm.o

handler_asm.o :
	$(AS) $(INCLUDES) handler.s -o handler_asm.o

clean:
	rm -rf *~ *.o

include ../include.mk
