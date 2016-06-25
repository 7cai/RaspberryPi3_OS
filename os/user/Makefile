INCLUDES	  := -I../include/ -I include/

user_dir      := ../../user

user.o:
	$(MAKE) --directory=$(user_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(user_dir)/user.c

.PHONY: clean

all: user.o

clean:
	rm -rf *~ *.o

include ../include.mk
