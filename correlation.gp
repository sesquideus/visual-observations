f(x) = a*x + b
g(x) = x 

a = 2
b = -2

set terminal pdf size 20cm, 20cm font "Minion Pro, 20"
set title "AMOS vs visual observation calibration"
set output 'correlation.pdf'
set size ratio -1
set xlabel "AMOS magnitude / 1"
set xtics 1
#set xtics textcolor rgb "008080"
set ytics 1
set grid lc rgb '#808080'
set key bottom right height 2 width 0.3 box lw 1
set ylabel "visual magnitude / 1"
fit f(x) 'all.corr' using 2:3 via a, b
set yrange [-6:6]
set xrange [-5:5]

set style fill transparent solid 0.333 noborder
set style circle radius 0.05

plot 'all.corr' using ($4 == 0 ? 1/0 : $2):($5 == '00' ? 1/0 : $3) with circles lc rgb '#004080' title 'Data points', \
    f(x) ls 1 lc rgb "#004080" lw 2 title 'fit', \
    g(x) ls 2 lc rgb "#8000FF" lw 2 title 'y = x'

