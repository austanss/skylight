ENTRY(boot)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
	. = 0xffffffff80000000 + 2M;

	.stivalehdr : {
        KEEP(*(.stivalehdr))
    }

	.text : ALIGN(4k) {
		*(.text*)
	}

	.data : ALIGN(4k) {
		*(.data*)
	}

	.rodata : {
		*(.rodata*)
	}

	.bss : ALIGN(4k) {
		*(COMMON)
		*(.bss*)
	}
}