CROSS_COMPILE	:= aarch64-none-elf
#CROSS_COMPILE	:= ~/gcc-linaro-aarch64-linux-gnu-4.8-2013.10_linux/bin/aarch64-linux-gnu
CC		:= $(CROSS_COMPILE)-gcc
CFLAGS		:= -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
LD		:= $(CROSS_COMPILE)-ld
AS		:= $(CROSS_COMPILE)-as
OBJCOPY		:= $(CROSS_COMPILE)-objcopy
OBJDUMP		:= $(CROSS_COMPILE)-objdump
