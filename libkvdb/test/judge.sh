#!/bin/bash
make clean
make run TEST_FILE=write <in
make test
