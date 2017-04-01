f(x) = a*x + b
g(x) = x 
expo(x) = c*(exp(d*x) + e) + j*x + k
c = -0.2
d = -1
e = -5
j = 0.75
k = 1

set terminal pdf size 20cm, 20cm font "Minion Pro, 20"
set title "AMOS vs visual observation calibration"
set output 'correlation.pdf'
set size ratio -1
set xlabel "AMOS magnitude / 1"
set xtics 1
set ytics 1
set grid lc rgb '#000000'
set key bottom right height 2 width 0.3 box lw 1
set ylabel "visual magnitude / 1"
fit f(x) 'all.corr' using 2:3 via a, b
fit expo(x) 'all.corr' using 2:3:(1):(1) xyerror via c, d, e, j, k

set yrange [-5:5]
set xrange [-5:5]

set style fill transparent solid 0.333 noborder
set style circle radius 0.05

plot 'all.corr' using ($4 == 0 ? 1/0 : $2):($5 == '00' ? 1/0 : $3) with circles lc rgb '#002080' title 'data points', \
    x ls 2 lc rgb "#6000FF" lw 2 title 'equivalence', \
    f(x) ls 1 lc rgb "#0040A0" lw 2 title 'linear fit', \
    expo(x) ls 1 lc rgb "#00C000" lw 2 title 'linear + exponential fit'
