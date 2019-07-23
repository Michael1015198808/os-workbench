NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c" -o -name "*.S")
INC_DIR        := include/ framework/
export AM_HOME := $(PWD)/../abstract-machine
cores := 2
ifeq ($(ARCH),)
export ARCH := x86-qemu
endif

PREBUILD := git
include $(AM_HOME)/Makefile.app
include ../Makefile.lab

.PHONY: initrd

initrd:
	@echo ramdisk0 updated!
	@gcc -o initrd/initrd initrd/initrd.c
	@initrd/initrd > build/ramdisk.img
#@seq 1 8192 > build/ramdisk.img


QEMU_FLAGS := -serial stdio -machine accel=tcg -drive format=raw,file=$(BINARY)

run1: image
	qemu-system-i386 -smp 1 $(QEMU_FLAGS)

run2: image
	qemu-system-i386 -smp 2 $(QEMU_FLAGS)

run4: image
	qemu-system-i386 -smp 4 $(QEMU_FLAGS)

run6: image
	qemu-system-i386 -smp 6 $(QEMU_FLAGS)

gdb : 
	@gdb -ex "target remote 127.0.0.1:1234" -ex "symbol $(BINARY).o"
debug: image
	qemu-system-i386 -smp $(cores) $(QEMU_FLAGS) -S -s -d guest_errors -monitor telnet:127.0.0.1:1111,server,nowait 
