reset
n=100
max=100
min=0
width=(max-min)/n
hist(x,width)=width*floor(x/width)+width/2.0
set terminal png size 500,500
set output 'hist.png'
set xrange [min:max]
set yrange[0:]
set offset graph 0.05,0.05,0.05,0.0
set xtics min,(max-min)/5,max
set boxwidth width*0.9
set style fill solid 0.5
set tics out nomirror
set title "Markovian SIR Model Time Period"
set xlabel "Time"
set ylabel "Frequency"
plot "data" using (hist($2,width)):(1.0) smooth freq with boxes lc rgb"green"
