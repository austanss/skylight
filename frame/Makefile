SOURCE	= src
OUTPUT	= build

CCODE 	= $(shell find $(SOURCE) -type f -name '*.c')
SCODE	= $(shell find $(SOURCE) -type f -name '*.s')
OBJECTS	= $(patsubst $(SOURCE)/%.c, $(OUTPUT)/%.c.o, $(CCODE))
OBJECTS += $(patsubst $(SOURCE)/%.s, $(OUTPUT)/%.s.o, $(SCODE))

FRAME	= $(OUTPUT)/frame.se

CC 	= clang
LD 	= clang
AS 	= nasm

CCFLAGS = \
-ffreestanding \
-I$(SOURCE) \
-mno-red-zone \
-fpic \
-gdwarf \
-Werror 

LDFLAGS = \
-nostdlib

ASFLAGS = \
-f elf64 \
-g \
-F dwarf

.DEFAULT-GOAL	= all
.PHONY			= clean

$(OUTPUT)/%.c.o: $(SOURCE)/%.c
	@ echo "	compile $^"
	@ mkdir -p $(@D)
	@ $(CC) $(CCFLAGS) -c $^ -o $@

$(OUTPUT)/%.s.o: $(SOURCE)/%.s
	@ echo "	assemble $^"
	@ mkdir -p $(@D)
	@ $(AS) $(ASFLAGS) $^ -o $@

$(FRAME): $(OBJECTS)
	@ echo "	link $@"
	@ $(LD) $(LDFLAGS) $(OBJECTS) -o $(FRAME)

clean:
	@ rm -rf build

all: $(FRAME)