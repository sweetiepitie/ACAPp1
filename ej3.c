#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>

#define MASTER 0
#define TAG 0

double get_wall_time(){
	struct timeval t;
	if (gettimeofday(&t, NULL)){
		printf("lol\n");
		return 0;
	}

	return (double)t.tv_sec + (double)t.tv_usec * .000001;
}

double piLeibniz(int steps, int l_limit, int u_limit){
	double partpi = 0.0;
	double num = 1.0;
	double denom = 1.0;
	for(int i = l_limit; i < u_limit; i++){
		partpi += num/denom;
		num = -1.0*num; // Alternamos el signo
		denom += 2.0;
	}

	return 4.0*partpi;
}

double piRectangles(int intervals, int l_interval, int u_interval){
	double width = 1.0/intervals;
	double sum = 0.0, x;
	for(int i = l_interval; i<u_interval; i++){
		x = (i + 0.5)*width;
		sum += 4.0/(1.0 + x*x);
	}
	
    return sum*width;
}

void get_my_interval(int num_proc, int num_intervals, int id, int *l_limit, int *u_limit)
{
    int intervals_pprocess = num_intervals / num_proc,
        rest = num_intervals % num_proc;
    
    int rel_interval = (id == num_proc - 1) ? intervals_pprocess + rest : intervals_pprocess;

    *l_limit = id*intervals_pprocess;
    *u_limit = id*intervals_pprocess + rel_interval;
}

int main(int argc, char **argv)
{
    int size, rank;
    int intervals;
    if (argc != 2){
		printf("\n\nUse: mpiexec -n num_processes prog intervals\n");
        exit(-1);
    }
    intervals = atoi(argv[1]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int l_limit, u_limit;
    double part_pi;
    if (!rank){
        // Master
        double global_pi;
        double wall0, wall1;
        wall0 = get_wall_time();

        get_my_interval(size, intervals, rank, &l_limit, &u_limit);
        part_pi = piRectangles(intervals, l_limit, u_limit);
        MPI_Reduce(&part_pi, &global_pi, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
        
        wall1 = get_wall_time();

        printf("\n\nPI calculated by rectangles : %f\n", global_pi);
        printf("Wall time consumed : %f\n\n", wall1 - wall0);

        MPI_Barrier(MPI_COMM_WORLD);
        
        wall0 = get_wall_time();
        part_pi = piLeibniz(intervals, l_limit, u_limit);
        MPI_Reduce(&part_pi, &global_pi, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
        global_pi /= size;
        wall1 = get_wall_time();

        printf("\n\nPI calculated by leibnizs : %f\n", global_pi);
        printf("Wall time consumed : %f\n\n", wall1 - wall0);

    }else{
        // Their bithces
        get_my_interval(size, intervals, rank, &l_limit, &u_limit);
        part_pi = piRectangles(intervals, l_limit, u_limit);
        MPI_Reduce(&part_pi, NULL, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        part_pi = piLeibniz(intervals, l_limit, u_limit);
        MPI_Reduce(&part_pi, NULL, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    }

    MPI_Finalize();

    return 0;
}