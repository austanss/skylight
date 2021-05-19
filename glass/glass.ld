ENTRY(boot)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
	. = 0xffffffff80000000 + 2M;

	__code_base = .;

	.stivalehdr : {
        KEEP(*(.stivalehdr))
    }

	.text : ALIGN(0x40) {
		*(.text*)
	}
	
	__code_end = .;

	.data : ALIGN(0x40) {
		*(.data*)
	}

	.rodata : ALIGN(0x40) {
		*(.rodata*)
	}

	.bss : ALIGN(0x40) {
		*(COMMON)
		*(.bss*)
	}
}