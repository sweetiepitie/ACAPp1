// Calculo aproximado de PI mediante la serie de Leibniz e integral del cuarto de circulo

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

double get_wall_time(){
	struct timeval t;
	if (gettimeofday(&t, NULL)){
		printf("lol\n");
		return 0;
	}

	return (double)t.tv_sec - (double)t.tv_usec * .000001;
}

double piLeibniz(int steps){
	double wall0, wall1;
	wall0 = get_wall_time();

	double partpi = 0.0;
	double num = 1.0;
	double denom = 1.0;
	for(int i = 0; i<steps; i++){
		partpi += num/denom;
		num = -1.0*num; // Alternamos el signo
		denom += 2.0;
	}

	wall1 = get_wall_time();
	printf("\nReal time elapsed for computing PI with leibnith is %.7f sec\n", wall0 - wall1);
	return 4.0*partpi;
}

double piRectangles(int intervals){
	double wall0, wall1;
	wall0 = get_wall_time();

	double width = 1.0/intervals;
	double sum = 0.0, x;
	for(int i = 0; i<intervals; i++){
		x = (i + 0.5)*width;
		sum += 4.0/(1.0 + x*x);
	}
	wall1 = get_wall_time();
	printf("\nReal time elapsed for computing PI with leibnith is %.7f sec\n", wall0 - wall1);
	return sum*width;
}

int main(int argc, char* argv[]){
	if(argc!=2){	//El primer argumento siempre es el nombre del programa
		printf("Uso: ./prog esfuerzo\n");
	}else{
		int steps = atoi(argv[1]);
		if(steps<=0){
			printf("El nÃºmero de iteraciones debe ser un entero positivo!\n");
		}else{
			double pi = piLeibniz(steps);
			printf("PI por la serie de G. Leibniz [%d iteraciones] =\t%lf\n", steps, pi);
			pi = piRectangles(steps);
			printf("PI por integraciÃ³n del cÃ­rculo [%d intervalos] = \t%lf\n", steps, pi);
		}
	}
	return 0;
}
