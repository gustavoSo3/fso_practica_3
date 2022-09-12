#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>

#define N_ITERATIONS 2000000000 // 2 mill millones

double numerador = 4;

struct chunkData
{
	double denominador;
	int sign;
	int begining;
	int end;
};

double chunkOfPI(struct chunkData data)
{

	double local_pi = 0.0;
	for (int i = data.begining; i < data.end; i++)
	{
		local_pi += data.sign * (numerador / data.denominador);
		data.denominador += 2;
		data.sign *= -1;
	}
	return local_pi;
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
	double *shared_mem_pi;

	int shmid = shmget(0x160700, sizeof(double) * n_process, 0666 | IPC_CREAT);
	if (shmid == -1)
	{
		exit(1);
	}

	shared_mem_pi = shmat(shmid, NULL, 0);

	double pi = 0;

	for (int i = 0; i < n_process; i++)
	{

		int chunk_size = (N_ITERATIONS / n_process);
		arguments[i].begining = chunk_size * i;
		arguments[i].end = (chunk_size * i) + chunk_size;
		arguments[i].sign = (arguments[i].begining % 2) ? -1 : 1;
		arguments[i].denominador = 1 + (arguments[i].begining * 2);

		printf("%d, %d, %d, %f\n", arguments[i].begining, arguments[i].end, arguments[i].sign, arguments[i].denominador);
		process[i] = fork();
		if (process[i] == 0)
		{
			shared_mem_pi[i] = chunkOfPI(arguments[i]);
			exit(0);
		}
	}

	for (int i = 0; i < n_process; i++)
	{
		wait(NULL);
	}

	for (int i = 0; i < n_process; i++)
	{
		pi += shared_mem_pi[i];
	}

	printf("PI: %f\n", pi);

	shmdt(shared_mem_pi);
	shmctl(shmid, IPC_RMID, NULL); // IPC_RMID marks the segment to be destroy

	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);
	return 0;
}