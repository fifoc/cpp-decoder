#!/bin/bash
echo "Building fif2png"
g++ -c fif2png.cpp -O2 -Wall `libpng-config --cflags`
g++ fif2png.o -o fif2png `libpng-config --ldflags`
rm fif2png.o
echo "Building fifsdl"
g++ fifsdl.cpp -O2 -Wall -o fifsdl -lSDL2
