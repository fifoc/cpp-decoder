#!/bin/bash
echo "Building fif2png..."
g++ -c fif2png.cpp -O2 -Wall -Wextra `libpng-config --cflags`
g++ fif2png.o -o fif2png `libpng-config --ldflags`
rm fif2png.o &> /dev/null

echo "Building fifsdl..."
g++ fifsdl.cpp -O2 -Wall -Wextra -o fifsdl -lSDL2
