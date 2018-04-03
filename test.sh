#!/bin/bash

# Executable definition
EXEC="./fft -t FFT"
if [ ! -f $EXEC ]; then
    echo "Unable to find executable. Run:"
    echo "    make"
    echo "    $0"
fi

cat sample/sine.data | $EXEC > sample/spectrum.out
gnuplot --persist sample/spectrum.gnuplot
