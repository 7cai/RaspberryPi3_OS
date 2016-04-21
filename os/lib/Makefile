
INCLUDES	  := -I include/


%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: print.o sleep.o

clean:
	rm -rf *~ *.o

include ../include.mk
