#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "helpers.h"

#include "strategies/fmutex.h"
#include "strategies/adaptive_mutex.h"
#include "strategies/mcs_lock.h"

static fmutex_t mutex = FMUTEX_INIT;
static ad_mutex_t ad_mutex = AD_MUTEX_INIT;
static mcs_lock_t mcs_lock = MCS_LOCK_INIT;

#define NUM_THREADS 20


typedef struct {
    int thread_id;
} thread_arg_t;

void *request(void *arg)
{
    thread_arg_t *targ = (thread_arg_t *)arg;
    printf("[THREAD %d] Processing request...\n", targ->thread_id);

    busy_wait_ns(500000);

    printf("[THREAD %d] Finished\n", targ->thread_id);
}

void run(int num_threads)
{
    pthread_t threads[num_threads];
    thread_arg_t args[num_threads];
    struct timespec t_start, t_end;

    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int i = 0; i < num_threads; i++) 
    {
        args[i].thread_id = i;
        if (pthread_create(&threads[i], NULL, request, &args[i]) != 0) 
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);

    double elapsed = timespec_diff_sec(t_start, t_end);
}