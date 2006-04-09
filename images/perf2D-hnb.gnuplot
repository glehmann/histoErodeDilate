set term postscript eps enhanced color
#set output "perf2D-hnb.eps"
set xlabel "Number of pixels added and removed per structuring element translation"
set ylabel "Execution time (seconds)"
plot  [] [0:20] "../images/perf2D.txt" using 5:6 with linespoints title "Basic algorithm (dilation)", \
  "../images/perf2D.txt" using 5:7 with linespoints title "Moving histogram (dilation)",\
  "../images/perf2D.txt" using 5:13 with linespoints title "Moving histogram (gradient)"\

