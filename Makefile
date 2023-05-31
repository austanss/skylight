OUTPUT	= build
GLASS	= $(OUTPUT)/glass.sys
FRAME	= $(OUTPUT)/frame.se
IMAGE	= $(OUTPUT)/skylight.hdd

.DEFAULT-GOAL	= image
.PHONY			= clean

$(GLASS):
	@ make -C glass -j10
	@ mkdir -p build
	@ cp glass/build/glass.sys $(GLASS)

$(FRAME):
	@ make -C frame -j10
	@ mkdir -p build
	@ cp frame/build/frame.se $(FRAME)

$(IMAGE): glass frame
	@ wget https://github.com/limine-bootloader/limine/raw/v3.0-branch-binary/BOOTX64.EFI --quiet
	@ dd if=/dev/zero of=fat.img bs=1M count=128
	@ mformat -i fat.img -F ::
	@ mmd -i fat.img ::/efi
	@ mmd -i fat.img ::/efi/boot
	@ mmd -i fat.img ::/boot
	@ mmd -i fat.img ::/sys
	@ mmd -i fat.img ::/sys/start
	@ mmd -i fat.img ::/sys/share
	@ mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
	@ mcopy -i fat.img $(GLASS) ::/sys/start/glass.sys
	@ mcopy -i fat.img $(FRAME) ::/sys/start/frame.se
	@ mcopy -i fat.img boot.cfg ::/boot/limine.cfg
	@ rm BOOTX64.EFI
	@ mv fat.img build/skylight.hdd

glass: $(GLASS)
frame: $(FRAME)
image: $(IMAGE)

clean:
	@ rm -rf build
	@ make -C glass clean
	@ make -C frame clean

run:
	@ qemu-system-x86_64 -bios /usr/share/ovmf/x64/OVMF.fd -drive file=$(IMAGE),format=raw -net none -m 512M -serial stdio -machine q35 $(QEMU_ARGS)

debug:
	@ qemu-system-x86_64 -bios /usr/share/ovmf/x64/OVMF.fd -drive file=$(IMAGE),format=raw -net none -m 512M -S -gdb tcp::1234 -machine q35
