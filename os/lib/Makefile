# Main makefile
#
# Copyright (C) 2007 Beihang University
# Written by Zhu Like ( zlike@cse.buaa.edu.cn )
#

# ========= Configuration =========

INCLUDES	  := -I include/

# ======== End of configuration ===

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: print.o printf.o

clean:
	rm -rf *~ *.o

include ../include.mk
