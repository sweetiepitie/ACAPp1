# ACAPp1

Compilation:
	ej1.c > mpicc ej1.c -o ej1
	
	pi.c  > gcc pi.c -o pi
	
	ej3.c > mpicc ej3.c -o ej3

Execution:
	ej1 > mpiexec --oversubscribe -n grade_of_parallelism ej1
	
	pi  > ./pi number_of_intervals
	
	ej3 > mpiexec --oversubscribe -n grade_of_parallelism ej3 number_of_intervals
	

Notes: 
	"--oversubscribe" option will allow to exceed number of available phisical slots for parallelism on your computer, no real CPU will be used.
