f(x) = a*x + b
g(x) = x 

set terminal pdf size 20cm, 20cm font "Minion Pro, 20" 
n=20 #number of intervals
max=5. #max value
min=-5. #min value
width=(max-min)/n #interval width
#function used to map a value to the intervals
hist(x,width)=width*floor(x/width)+width/2.0
set boxwidth width

set style line 1 lt 1 lc rgb "blue"


set xrange [-5:5]
set yrange [0:60]
set size ratio 1
set style fill solid border rgb "#00A000"
plot 'amos.out' using (hist($3,width)):(1.0) smooth frequency with boxes lc rgb "green" title 'Histogram'
