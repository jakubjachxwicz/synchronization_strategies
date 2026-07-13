// #define FUTEX_MUTEX
#define ADAPTIVE_MUTEX


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "strategies/fmutex.h"
#include "strategies/adaptive_mutex.h"

#define NUM_THREADS 8
#define ITERS 1000000

static fmutex_t mutex = FMUTEX_INIT;
static ad_mutex_t ad_mutex = AD_MUTEX_INIT;

static long shared_counter = 0;

typedef struct {
    int thread_id;
} thread_arg_t;

void *worker(void *arg)
{
    thread_arg_t *targ = (thread_arg_t *)arg;
    printf("Thread %d starting...\n", targ->thread_id);

    for (int i = 0; i < ITERS; i++) 
    {
        #ifdef FUTEX_MUTEX
        fmutex_lock(&mutex);
        shared_counter++;
        fmutex_unlock(&mutex);

        #elifdef ADAPTIVE_MUTEX
        am_lock(&ad_mutex);
        shared_counter++;
        am_unlock(&ad_mutex);
        #endif
    }

    printf("Thread %d finished\n", targ->thread_id);
    return NULL;
}

static double timespec_diff_sec(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec)
         + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void) 
{
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];
    struct timespec t_start, t_end;

    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int i = 0; i < NUM_THREADS; i++) 
    {
        args[i].thread_id = i;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) 
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);

    double elapsed = timespec_diff_sec(t_start, t_end);
    
    long expected = NUM_THREADS * ITERS;

    printf("------------------------\n");
    printf("Expected counter value: %ld\n", expected);
    printf("Counter value: %ld\n", shared_counter);
    printf("Time: %.4f s\n", elapsed);
    printf("Operations per second: %.0f\n", expected / elapsed);

    return 0;
}