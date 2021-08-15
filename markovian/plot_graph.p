reset
set terminal png size 500,500
set output "graph.png"
set title "Markovian SIR Model Time Period"
set xlabel "Time"
set ylabel "Frequency"
plot "data"
