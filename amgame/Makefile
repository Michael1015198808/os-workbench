NAME := amgame
SRCS := $(shell find -L ./src/ -name "*.c")
export TASK    := L0
export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86-qemu
endif

include $(AM_HOME)/Makefile.app
include ../Makefile.lab
image: git
