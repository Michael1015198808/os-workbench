NAME := $(shell basename $(PWD))
export TASK := M5
all: $(NAME)-64 $(NAME)-32

include ../Makefile
include ../Makefile.lab
.PHONY: run
BITS := 64
IMG := "test.img"
run: all
	./$(NAME)-$(BITS) $(IMG)
gdb:all
	gdb ./$(NAME)-$(BITS)
