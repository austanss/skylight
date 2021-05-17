OUTPUT	= build
GLASS	= $(OUTPUT)/glass.elf
IMAGE	= $(OUTPUT)/skylight.hdd

.DEFAULT-GOAL	= image
.PHONY			= clean

$(GLASS):
	@ make -C glass
	@ mkdir -p build
	@ mv glass/build/glass.elf $(GLASS)

$(IMAGE): glass
	@ mkdir -p image
	@ mkdir -p image/EFI
	@ mkdir -p image/EFI/BOOT
	@ mkdir -p image/sys
	@ mkdir -p image/sys/start
	@ wget https://github.com/limine-bootloader/limine/raw/latest-binary/BOOTX64.EFI
	@ mv BOOTX64.EFI image/EFI/BOOT/BOOTX64.EFI
	@ cp $(GLASS) image/sys/start/glass.sys
	@ cp boot.cfg image/limine.cfg
	@ ./f32pack.sh $(IMAGE) 260 image
	@ rm -rf image

glass: $(GLASS)
image: $(IMAGE)

clean:
	@ rm -rf build
	@ make -C glass clean

run:
	@ qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive file=$(IMAGE),format=raw -net none