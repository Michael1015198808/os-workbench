NAME := amgame
SRCS := $(shell find -L ./src/ -name "*.c")
ifeq ($(ARCH),)
export ARCH := x86-qemu
endif

include $(AM_HOME)/Makefile.app
include ../Makefile.lab
image: git
