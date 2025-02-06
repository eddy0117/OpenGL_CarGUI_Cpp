set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'cpu_usage_plot.png'

set title "CPU Usage Comparison Across Multiple Tests"
set xlabel "Time (Seconds)"
set ylabel "CPU Usage (%)"

set grid
set key outside

# 每次測試以不同顏色繪製
plot for [i=0:*] "tmp_1/cpu_usage_plot_data.txt" index i using 1:2 with lines title sprintf("Before - %d", i+1) lw 2, \
     for [i=0:*] "tmp_2/cpu_usage_plot_data.txt" index i using 1:2 with lines title sprintf("After - %d", i+1) lw 2