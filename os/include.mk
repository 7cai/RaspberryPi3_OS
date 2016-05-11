CROSS_COMPILE	:= aarch64-none-elf
CC		:= $(CROSS_COMPILE)-gcc
CFLAGS		:= -Wall -nostdlib -nostartfiles -ffreestanding -O2
LD		:= $(CROSS_COMPILE)-ld
AS		:= $(CROSS_COMPILE)-as
OBJCOPY		:= $(CROSS_COMPILE)-objcopy
OBJDUMP		:= $(CROSS_COMPILE)-objdump
