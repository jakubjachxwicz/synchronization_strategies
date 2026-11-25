#ifndef BENCHMARK_H
#define BENCHMARK_H

#define _GNU_SOURCE

#include "../strategies/sync_interface.h"

#define FAIRNESS_MEASUREMENT_MS 600
#define TIMEOUT_S 15

typedef struct {
    int num_threads;
    long iters_per_thread;
    long critical_section_ns;
    long non_critical_ns;
} benchmark_config_t;

typedef struct {
    double elapsed_sec;
    double ops_per_sec;
    double fairness;
    long final_counter;
    long expected_counter;
    int correctness_ok;
} benchmark_result_t;

benchmark_result_t run_benchmark(const sync_strategy_t *strategy,
                                  const benchmark_config_t *cfg);

#endif