KERNELFILE = iso/boot/kernel.elf
LIMINE_URL_BIOSSYS = "https://github.com/limine-bootloader/limine/raw/v5.x-branch-binary/limine-bios.sys"
LIMINE_URL_BIOS = "https://github.com/limine-bootloader/limine/raw/v5.x-branch-binary/limine-bios-cd.bin"
LIMINE_URL_UEFI = "https://github.com/limine-bootloader/limine/raw/v5.x-branch-binary/limine-uefi-cd.bin"
LIMINE_PATH_PREFIX = iso/
LIMINE_PATH_RELATIVE_BIOSSYS = boot/limine-bios.sys
LIMINE_PATH_BIOSSYS = $(LIMINE_PATH_PREFIX)$(LIMINE_PATH_RELATIVE_BIOSSYS)
LIMINE_PATH_RELATIVE_BIOS = boot/limine_bios.bin
LIMINE_PATH_BIOS = $(LIMINE_PATH_PREFIX)$(LIMINE_PATH_RELATIVE_BIOS)
LIMINE_PATH_RELATIVE_UEFI = boot/limine_uefi.bin
LIMINE_PATH_UEFI = $(LIMINE_PATH_PREFIX)$(LIMINE_PATH_RELATIVE_UEFI)
RESSRC = res/
RESDEST = iso/
SOURCES = $(shell find src/ -name "*.c" -o -name "*.s")
OBJECTS = ${subst .c,.o,${subst .s,.o,$(SOURCES)}}
ISOFILE = mdos.iso

install-dependencies:
	sudo apt install xorriso nasm gcc build-essential wget git coreutils grep sed mawk gzip nasm mtools gcc clang llvm
	git clone https://github.com/limine-bootloader/limine.git --recursive
	cd limine && ./bootstrap && ./configure --enable-bios --enable-bios-cd --enable-uefi-cd --enable-uefi-x86-64 && $(MAKE) && sudo $(MAKE) install
	rm -rf limine

all: $(ISOFILE)
run: $(ISOFILE)
	qemu-system-x86_64 -cdrom $(ISOFILE)
$(ISOFILE): $(LIMINE_PATH_BIOS) $(LIMINE_PATH_UEFI) $(LIMINE_PATH_BIOSSYS) $(KERNELFILE) $(RESSRC)
	cp -R $(RESSRC) $(RESDEST)
	xorriso -as mkisofs -b $(LIMINE_PATH_RELATIVE_BIOS) \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot $(LIMINE_PATH_RELATIVE_UEFI) \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso -o $(ISOFILE)
	limine bios-install $(ISOFILE)

$(LIMINE_PATH_BIOSSYS):
	wget -O $(LIMINE_PATH_BIOSSYS) $(LIMINE_URL_BIOSSYS)
$(LIMINE_PATH_BIOS):
	wget -O $(LIMINE_PATH_BIOS) $(LIMINE_URL_BIOS)
$(LIMINE_PATH_UEFI):
	wget -O $(LIMINE_PATH_UEFI) $(LIMINE_URL_UEFI)

$(KERNELFILE): $(OBJECTS)
	ld -T link.ld -static $(OBJECTS) -o $(KERNELFILE)

src/%.o: src/%.s
	nasm -f elf64 $< -o $@
src/%.o: src/%.c
	gcc -m64 -fno-builtin -fno-stack-protector \
		-Wall -Wextra -Werror -Isrc/lib/ -Isrc/drivers/ -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(ISOFILE) $(KERNELFILE) limine/
clean-limine:
	rm -rf $(LIMINE_PATH_BIOS) $(LIMINE_PATH_UEFI) $(LIMINE_PATH_BIOSSYS)
clean-all: | clean clean-limine