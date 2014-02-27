#!/bin/sh
export CC=g++
export CERO_PATH=.
export CERO_EXE=example
export CFLAGS="-std=c++11 -Wall"
export LFLAGS="-std=c++11 -lm -lpthread"

cero b main.ce



