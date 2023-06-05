ENTRY(_start64)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
	. = 0xffffffff80000000 + 2M;

	__load_base = .;

	.stivalehdr : {
        KEEP(*(.stivalehdr))
    }

	.text : ALIGN(0x1000) {
		*(.text*)
	}

	.data : ALIGN(0x1000) {
		*(.data*)
	}

	.rodata : ALIGN(0x1000) {
		*(.rodata*)
	}

	.bss : ALIGN(0x1000) {
		*(COMMON)
		*(.bss*)
	}
	
	. = ALIGN(0x1000);
	
	__load_max = .;
}