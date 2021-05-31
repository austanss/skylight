OUTPUT	= build
GLASS	= $(OUTPUT)/glass.elf
IMAGE	= $(OUTPUT)/skylight.hdd

.DEFAULT-GOAL	= image
.PHONY			= clean

$(GLASS):
	@ make -C glass -j10
	@ mkdir -p build
	@ cp glass/build/glass.elf $(GLASS)

$(IMAGE): glass
	@ wget https://github.com/limine-bootloader/limine/raw/latest-binary/BOOTX64.EFI --quiet
	@ dd if=/dev/zero of=fat.img bs=1M count=128
	@ mformat -i fat.img -F ::
	@ mmd -i fat.img ::/EFI
	@ mmd -i fat.img ::/EFI/BOOT
	@ mmd -i fat.img ::/sys
	@ mmd -i fat.img ::/sys/start
	@ mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
	@ mcopy -i fat.img $(GLASS) ::/sys/start/glass.sys
	@ mcopy -i fat.img boot.cfg ::/limine.cfg
	@ rm BOOTX64.EFI
	@ mv fat.img build/skylight.hdd

glass: $(GLASS)
image: $(IMAGE)

clean:
	@ rm -rf build
	@ make -C glass clean

run:
	@ qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive file=$(IMAGE),format=raw -net none -m 512M -serial stdio -machine q35