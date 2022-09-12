#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>

#define N_ITERATIONS 2000000000 // 2 mill millones
#define STACK_SIZE 1024 * 64

double numerador = 4;
double *pi_chunks;

struct chunkData
{
	double denominador;
	int sign;
	int begining;
	int end;
	int n_chunk;
};

int chunkOfPI(void *args)
{
	struct chunkData data = *((struct chunkData *)args);
	double local_pi = 0.0;
	for (int i = data.begining; i < data.end; i++)
	{
		local_pi += data.sign * (numerador / data.denominador);
		data.denominador += 2;
		data.sign *= -1;
	}
	pi_chunks[data.n_chunk] += local_pi;
	exit(0);
}

int main(int argc, char const *argv[])
{
	long long start_ts;
	long long stop_ts;
	long long elapsed_time;
	struct timeval ts;

	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	int n_process = sysconf(_SC_NPROCESSORS_ONLN);
	pid_t process[n_process];
	struct chunkData arguments[n_process];
	pi_chunks = malloc(sizeof(double) * n_process);
	void *stack = malloc(STACK_SIZE * n_process);

	double pi = 0;

	for (int i = 0; i < n_process; i++)
	{

		int chunk_size = (N_ITERATIONS / n_process);
		arguments[i].begining = chunk_size * i;
		arguments[i].end = (chunk_size * i) + chunk_size;
		arguments[i].sign = (arguments[i].begining % 2) ? -1 : 1;
		arguments[i].denominador = 1 + (arguments[i].begining * 2);
		arguments[i].n_chunk = i;

		printf("%d, %d, %d, %f\n", arguments[i].begining, arguments[i].end, arguments[i].sign, arguments[i].denominador);
		process[i] = clone(chunkOfPI, (char *)stack + (STACK_SIZE * (i + 1)), SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &arguments[i]);
	}

	for (int i = 0; i < n_process; i++)
	{
		wait(NULL);
	}

	for (int i = 0; i < n_process; i++)
	{
		pi += pi_chunks[i];
	}

	printf("PI: %f\n", pi);

	free(pi_chunks);
	free(stack);

	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);
	return 0;
}