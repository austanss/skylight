SOURCE	= src
OUTPUT	= build

CCODE 	= $(shell find $(SOURCE) -type f -name '*.c')
SCODE	= $(shell find $(SOURCE) -type f -name '*.s')
OBJECTS	= $(patsubst $(SOURCE)/%.c, $(OUTPUT)/%.cc.o, $(CCODE))
OBJECTS += $(patsubst $(SOURCE)/%.s, $(OUTPUT)/%.as.o, $(SCODE))

LDS = glass.ld

GLASS	= $(OUTPUT)/glass.elf

CC 	= clang
LD 	= clang
AS 	= nasm

CCFLAGS = \
-ffreestanding \
-I$(SOURCE) \
-mno-red-zone \
-mcmodel=kernel \
-fpie \
-gdwarf \
-fwrapv \
-Werror \
-pipe \
-nostdinc++

LDFLAGS = \
-nostdlib \
-Wl,-T$(LDS) \
-mcmodel=kernel \
-pie

ASFLAGS = \
-f elf64 \
-g \
-F dwarf

.DEFAULT-GOAL	= all
.PHONY			= clean

$(OUTPUT)/%.cc.o: $(SOURCE)/%.c
	@ echo "	compile $^"
	@ mkdir -p $(@D)
	@ $(CC) $(CCFLAGS) -c $^ -o $@

$(OUTPUT)/%.as.o: $(SOURCE)/%.s
	@ echo "	assemble $^"
	@ mkdir -p $(@D)
	@ $(AS) $(ASFLAGS) $^ -o $@

$(GLASS): $(OBJECTS)
	@ echo "	link $@"
	@ $(LD) $(LDFLAGS) $(OBJECTS) -o $(GLASS)

clean:
	@ rm -rf build

all: $(GLASS)