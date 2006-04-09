set term postscript eps enhanced color
#set output "perf_nb_of_neighbors.eps"
set xlabel "Image size (pixels)"
set ylabel "Execution time (seconds)"
plot [0:] [0:] "../images/perf_nb_of_neighbors.txt" using 4:6 with linespoints title "Basic algorithm", \
  "../images/perf_nb_of_neighbors.txt" using 4:7 with linespoints title "Moving histogram algorithm"\

