
INCLUDES	  := -I include/


%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: print.o

clean:
	rm -rf *~ *.o

include ../include.mk
