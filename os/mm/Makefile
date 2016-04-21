
INCLUDES	  := -I./include/ -I../lib/include/ -I../env/include/ -I../include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: pmap.o

clean:
	rm -rf *~ *.o


include ../include.mk
