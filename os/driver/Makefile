
INCLUDES	  := -I../lib/include/

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: console.o

clean:
	rm -rf *~ *.o

include ../include.mk
