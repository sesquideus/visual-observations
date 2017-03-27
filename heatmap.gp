set format x "" 
set format y "" 
unset key
set view map
set style data lines
set xtics border in scale 0,0 mirror norotate  autojustify
set ytics border in scale 0,0 mirror norotate  autojustify
set ztics border in scale 0,0 nomirror norotate  autojustify
unset cbtics
set rtics axis in scale 0,0 nomirror norotate  autojustify
set title "plot with image pixels" 
set xrange [ -0.00000 : 15.0000 ] noreverse nowriteback
set yrange [ -0.00000 : 15.0000 ] noreverse nowriteback
set cblabel "Score" 
set cbrange [ 0.00000 : 55.00000 ] noreverse nowriteback
set palette rgbformulae -7, 2, -7
unset colorbox
x = 0.0
## Last datafile plotted: "$map1"
plot "heatmap.txt" matrix using 1:2:3 with image pixels
