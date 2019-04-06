NAME := $(shell basename $(PWD))
export TASK := M3
all: $(NAME)-64 $(NAME)-32

include ../Makefile
include ../Makefile.lab
.PHONY: test_64
test_64: sperf-64
	@./sperf-64 tree / 2>/dev/null
