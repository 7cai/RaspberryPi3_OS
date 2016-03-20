
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

bootloader07.o :
	$(ARMGNU)-gcc $(COPS) -c boot/bootloader07.c -o boot/bootloader07.o

periph7.o :
	$(ARMGNU)-gcc $(COPS) -c driver/periph.c -o driver/periph7.o

kernel7.img : loader vectors.o periph7.o bootloader07.o 
	$(ARMGNU)-ld boot/vectors.o driver/periph7.o boot/bootloader07.o -T loader -o bootloader07_rpi2.elf
	$(ARMGNU)-objcopy bootloader07_rpi2.elf -O binary kernel7.img
