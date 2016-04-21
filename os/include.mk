# Common includes in Makefile
#
# Copyright (C) 2007 Beihang University
# Written By Zhu Like ( zlike@cse.buaa.edu.cn )


CROSS_COMPILE	:= aarch64-none-elf
CC		:= $(CROSS_COMPILE)-gcc
CFLAGS		:= -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
LD		:= $(CROSS_COMPILE)-ld
AS		:= $(CROSS_COMPILE)-as
OBJCOPY		:= $(CROSS_COMPILE)-objcopy
OBJDUMP		:= $(CROSS_COMPILE)-objdump
