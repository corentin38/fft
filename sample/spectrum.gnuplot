#définition de quelques variables
	xmin=0.
	xmax=1024.
	ymin=0.
	ymax=300.
    x10=(xmax-xmin)*10/100
    y10=(ymax-ymin)*10/100
    x5=(xmax-xmin)*5/100
    y5=(ymax-ymin)*5/100


#initialisation du terminal
reset
set term x11
unset autoscale
set xr [xmin - x10:xmax + x10]
set yr [ymin - y10:ymax + y10]

#options
unset label

set title "FFT du signal d'entree"

#les axes
set arrow 3 from xmin,0 to xmax+x5,0
set arrow 4 from 0,ymin to 0,ymax

#l'origine
set label "0" at xmin - x5, ymin - y5

set label "freq" at xmax, -y5
set label "intensity" at x5, ymax - y5

plot "spectrum.out" title "spectre" w l lt 3 lw 2
