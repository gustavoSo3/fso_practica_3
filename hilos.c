#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define N_ITERATIONS 2000000000 // 2 mill millones

pthread_mutex_t mutex;

double numerador = 4;
double pi = 0;

struct chunkData
{
	double denominador;
	int sign;
	int begining;
	int end;
};

void *chunkOfPI(void *args)
{
	struct chunkData data = *((struct chunkData *)args);

	double local_pi = 0.0;
	for (int i = data.begining; i < data.end; i++)
	{
		local_pi += data.sign * (numerador / data.denominador);
		data.denominador += 2;
		data.sign *= -1;
	}
	pthread_mutex_lock(&mutex);
	pi = pi + local_pi;
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char const *argv[])
{
	long long start_ts;
	long long stop_ts;
	long long elapsed_time;
	struct timeval ts;

	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	int n_threads = sysconf(_SC_NPROCESSORS_ONLN);
	pthread_t threads[n_threads];
	struct chunkData arguments[n_threads];

	pthread_mutex_init(&mutex, NULL);

	for (int i = 0; i < n_threads; i++)
	{
		int chunk_size = (N_ITERATIONS / n_threads);
		arguments[i].begining = chunk_size * i;
		arguments[i].end = (chunk_size * i) + chunk_size;
		arguments[i].sign = (arguments[i].begining % 2) ? -1 : 1;
		arguments[i].denominador = 1 + (arguments[i].begining * 2);

		printf("%d, %d, %d, %f\n", arguments[i].begining, arguments[i].end, arguments[i].sign, arguments[i].denominador);
		pthread_create(&threads[i], NULL, chunkOfPI, &arguments[i]);
	}

	for (int i = 0; i < n_threads; i++)
	{
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	printf("PI: %f\n", pi);

	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);
	return 0;
}