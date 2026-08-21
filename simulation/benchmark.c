#include "benchmark.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

static long g_shared_counter;
static const sync_strategy_t *g_strategy;
static benchmark_config_t g_cfg;

static void busy_wait_ns(long ns)
{
    if (ns <= 0) return;
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    long elapsed;
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
        elapsed = (now.tv_sec - start.tv_sec) * 1000000000L
                + (now.tv_nsec - start.tv_nsec);
    } while (elapsed < ns);
}

static void *worker(void *arg)
{
    (void)arg;
    void *ctx = g_strategy->thread_ctx_create();

    for (long i = 0; i < g_cfg.iters_per_thread; i++) {
        g_strategy->lock(ctx);
        g_shared_counter++;
        busy_wait_ns(g_cfg.critical_section_ns);
        g_strategy->unlock(ctx);

        busy_wait_ns(g_cfg.non_critical_ns);  // symuluje pracę MIĘDZY sekcjami -
                                                // to reguluje "gęstość" kontencji
    }

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
    struct timespec t_start, t_end;

    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < cfg->num_threads; i++)
        pthread_create(&threads[i], NULL, worker, NULL);
    for (int i = 0; i < cfg->num_threads; i++)
        pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    strategy->global_destroy();
    free(threads);

    long expected = (long)cfg->num_threads * cfg->iters_per_thread;
    double elapsed = (t_end.tv_sec - t_start.tv_sec)
                    + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    return (benchmark_result_t){
        .elapsed_sec = elapsed,
        .ops_per_sec = expected / elapsed,
        .final_counter = g_shared_counter,
        .expected_counter = expected,
        .correctness_ok = (g_shared_counter == expected),
    };
}