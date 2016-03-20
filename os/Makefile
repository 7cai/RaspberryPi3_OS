
ARMGNU ?= aarch64-none-elf

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 

gcc : kernel7.img

all : gcc

clean :
	rm -f boot/*.o
	rm -f driver/*.o
	rm -f *.elf
	rm -f *.img

vectors.o :
	$(ARMGNU)-as boot/vectors.s -o boot/vectors.o

init.o :
	$(ARMGNU)-gcc $(COPS) -c boot/init.c -o boot/init.o

bootloader.o :
	$(ARMGNU)-gcc $(COPS) -c boot/bootloader.c -o boot/bootloader.o

console.o :
	$(ARMGNU)-gcc $(COPS) -c driver/console.c -o driver/console.o

kernel7.img : loader vectors.o console.o init.o bootloader.o
	$(ARMGNU)-ld boot/vectors.o driver/console.o boot/init.o boot/bootloader.o -T loader -o bootloader.elf
	$(ARMGNU)-objcopy bootloader.elf -O binary kernel7.img
