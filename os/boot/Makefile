# Main makefile
#
# Copyright (C) 2007 Beihang University
# Written by Zhu Like ( zlike@cse.buaa.edu.cn )
#

# ========= Configuration =========

INCLUDES	  := -I../lib/include/

# ======== End of configuration ===

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: asm.o bootloader.o init.o

asm.o :
	$(AS) asm.s -o asm.o

clean:
	rm -rf *~ *.o

include ../include.mk
