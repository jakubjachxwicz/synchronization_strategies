#include "benchmark.h"
#include "helpers.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define FAIRNESS_MEASUREMENT_MS 600

static long g_shared_counter;
static const sync_strategy_t *g_strategy;
static benchmark_config_t g_cfg;

static void busy_wait_ns(long ns)
{
    if (ns <= 0) return;
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    long elapsed;
    do 
    {
        clock_gettime(CLOCK_MONOTONIC, &now);
        elapsed = (now.tv_sec - start.tv_sec) * 1000000000L
                + (now.tv_nsec - start.tv_nsec);
    } while (elapsed < ns);
}

static inline long long now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline void worker_iteration(void *ctx)
{
    g_strategy->lock(ctx);
    g_shared_counter++;
    busy_wait_ns(g_cfg.critical_section_ns);
    g_strategy->unlock(ctx);

    busy_wait_ns(g_cfg.non_critical_ns);
}

static void *worker(void *arg)
{
    long *operation_counter = (long*)arg;
    void *ctx = g_strategy->thread_ctx_create();

    const long long deadline = now_ns() + (long long)(FAIRNESS_MEASUREMENT_MS * 1e6);
    long local_counter = 0;
    long i = 0;

    for (; i < g_cfg.iters_per_thread; i++) 
    {
        worker_iteration(ctx);
        if (now_ns() > deadline) { i++; break; }
        local_counter++;
    }

    for (; i < g_cfg.iters_per_thread; i++)
        worker_iteration(ctx);

    *operation_counter = local_counter;

    g_strategy->thread_ctx_destroy(ctx);
    return NULL;
}

benchmark_result_t run_benchmark(const sync_strategy_t *strategy,
                                  const benchmark_config_t *cfg)
{
    g_strategy = strategy;
    g_cfg = *cfg;
    g_shared_counter = 0;

    strategy->global_init();

    pthread_t *threads = malloc(sizeof(pthread_t) * cfg->num_threads);
    long *thread_operation_counter = malloc(sizeof(pthread_t) * cfg->num_threads);

    struct timespec t_start, t_end;
    int n = cfg->num_threads;

    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < n; i++)
        pthread_create(&threads[i], NULL, worker, &thread_operation_counter[i]);
    for (int i = 0; i < n; i++)
        pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    double left = (double)n * array_min(thread_operation_counter, n) / array_sum(thread_operation_counter, n);
    double right = array_sum(thread_operation_counter, n) / n / array_max(thread_operation_counter, n);
    // printf("n: %d, left: %ld, right: %ld, sum: %ld, min: %ld, max: %ld\n", n, left, right, array_sum(thread_operation_counter, n), array_min(thread_operation_counter, n), array_max(thread_operation_counter, n));
    double fairness = left < right ? left : right;

    strategy->global_destroy();
    free(threads);
    free(thread_operation_counter);

    long expected = (long)cfg->num_threads * cfg->iters_per_thread;
    double elapsed = (t_end.tv_sec - t_start.tv_sec)
                    + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    return (benchmark_result_t){
        .elapsed_sec = elapsed,
        .ops_per_sec = expected / elapsed,
        .fairness = fairness,
        .final_counter = g_shared_counter,
        .expected_counter = expected,
        .correctness_ok = (g_shared_counter == expected),
    };
}