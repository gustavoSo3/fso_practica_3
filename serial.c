#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define N_ITERATIONS 2000000000 // 2 mill millones

int main(int argc, char const *argv[])
{
	long long start_ts;
	long long stop_ts;
	long long elapsed_time;
	struct timeval ts;

	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	double numerador = 4;
	double denominador = 1; // A este lo vamos a ir aumentando de 2 en 2
	double pi = 0;
	int sign = 1;
	for (int i = 0; i < N_ITERATIONS; i++)
	{
		pi += sign * (numerador / denominador);
		denominador += 2;
		sign *= -1;
	}

	printf("PI: %f\n", pi);

	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);
	return 0;
}