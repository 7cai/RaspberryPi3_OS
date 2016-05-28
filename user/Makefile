link_script   := loader

INCLUDES	  := -I../os/lib/include/

objects       := *.o			\
				 ../os/user/syscall.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: all clean

all: clean $(objects) user

user:
	$(LD) $(objects) -T $(link_script) -e umain -o user.elf

clean:
	rm -rf *~ *.o

debug:
	$(OBJDUMP) -D -m aarch64 user.elf > user.list

include ../os/include.mk
