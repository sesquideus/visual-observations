set terminal pdf size 20cm, 20cm font "Minion Pro, 20"
set title "AMOS visual bins"
set output 'bins.pdf'
set xlabel "Apparent magnitude (bin width 0.5)"
set ylabel "Count"
set xtics 1
set ytics 10
set grid
set key top left height 2 box lw 1
set xrange [-5:7]

set style fill transparent solid 0.5 border -1
plot 'spolu-amos.bins' using 1:2 with boxes lc rgb 'forest-green' title "AMOS", \
    'spolu-visual.bins' using 1:2 with boxes lc rgb 'gold' title "Visual"
