NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c")
INC_DIR        := include/ framework/
export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86-qemu
endif

PREBUILD := git
include $(AM_HOME)/Makefile.app
include ../Makefile.lab

QEMU_FLAGS := -serial stdio -machine accel=kvm:tcg -drive format=raw,file=$(BINARY)

run2: image
	qemu-system-i386 -smp 2 $(QEMU_FLAGS)

run4: image
	qemu-system-i386 -smp 4 $(QEMU_FLAGS)
