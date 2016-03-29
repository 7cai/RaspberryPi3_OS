
driver_dir	  := driver
boot_dir	  := boot
lib_dir		  := lib

link_script   := loader

modules		  := boot driver lib
objects		  := $(boot_dir)/*.o		\
			   	 $(driver_dir)/*.o 		\
				 $(lib_dir)/*.o


.PHONY: all $(modules) clean

all: clean $(modules) kernel7

kernel7: $(modules)
	$(LD) $(objects) -T $(link_script) -e _start -o bootloader.elf
	$(OBJCOPY) bootloader.elf -O binary kernel7.img

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ kernel7.img bootloader.elf

debug:
	$(OBJDUMP) -D -m aarch64 bootloader.elf > bootloader.list

include include.mk
