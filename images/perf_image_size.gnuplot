set term postscript eps enhanced color
#set output "perf_image_size.eps"
set xlabel "Image size (pixels)"
set ylabel "Execution time (seconds)"
plot "../images/perf_image_size.txt" using 6:7 with linespoints title "Basic algorithm", \
  "../images/perf_image_size.txt" using 6:8 with linespoints title "Moving histogram algorithm"\

