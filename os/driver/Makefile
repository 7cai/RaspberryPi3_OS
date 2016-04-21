
INCLUDES	  := -I../lib/include/ -I include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: uart.o gpio.o

clean:
	rm -rf *~ *.o

include ../include.mk
