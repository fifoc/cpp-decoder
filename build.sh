#!/bin/bash
g++ -c fif2png.cpp -O2 -Wall `libpng-config --cflags`
g++ fif2png.o -o fif2png `libpng-config --ldflags`
rm fif2png.o
