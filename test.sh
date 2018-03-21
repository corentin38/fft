#!/bin/bash

# Executable definition
EXEC=fft
if [ ! -f $EXEC ]; then
    echo "Unable to find executable. Run:"
    echo "    make"
    echo "    $0"
fi

./$EXEC < sample/sine.data > sample/spectrum.out
gnuplot --persist sample/spectrum.gnuplot
