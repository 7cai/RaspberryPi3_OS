
INCLUDES	  := -I../lib/include/ -I include/ -I../include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: uart.o gpio.o timer.o

clean:
	rm -rf *~ *.o

include ../include.mk
