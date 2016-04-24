
driver_dir	  := driver
boot_dir	  := boot
lib_dir		  := lib
mm_dir		  := mm

link_script   := loader

modules		  := boot driver lib mm
objects		  := $(boot_dir)/*.o		\
			   	 $(driver_dir)/*.o 		\
				 $(lib_dir)/*.o 		\
				 $(mm_dir)/*.o


.PHONY: all $(modules) clean

all: clean $(modules) kernel8

kernel8: $(modules)
	$(LD) $(objects) -T $(link_script) -e _start -o bootloader.elf
	$(OBJCOPY) bootloader.elf -O binary kernel8.img

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ kernel8.img bootloader.elf

debug:
	$(OBJDUMP) -D -m aarch64 bootloader.elf > bootloader.list

include include.mk
