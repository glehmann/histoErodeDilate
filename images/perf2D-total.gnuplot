set term postscript eps enhanced color
#set output "perf2D.eps"
set xlabel "Structuring element size (pixels)"
set ylabel "Execution time (seconds)"
plot  [0:200] [0:] "../images/perf2D.txt" using 3:6 with linespoints title "Basic algorithm (dilation)", \
  "../images/perf2D.txt" using 3:7 with linespoints title "Moving histogram (dilation)",\
  "../images/perf2D.txt" using 3:13 with linespoints title "Moving histogram (gradient)"\

