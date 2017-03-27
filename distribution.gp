set terminal pdf size 20cm, 20cm font "Minion Pro, 20"
set title "AMOS and visual magnitude distribution"
set output 'distribution.pdf'
set xlabel "Apparent magnitude (bin width 0.5)"
set ylabel "Count"
set xtics 1
set ytics 10
set grid
set key top left height 2 box lw 1
set xrange [-5:7]

set style fill transparent solid 0.3 border -1
plot 'all-amos.bins' using 1:2 with boxes lc rgb '#0000FF' title "AMOS", \
    'all-visual.bins' using 1:2 with boxes lc rgb '#FF0000' title "Visual", \
    'all-amos-calib.bins' using 1:2 with boxes lc rgb '#00FF00' title "AMOS calibrated"
