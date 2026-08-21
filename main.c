#include <stdio.h>
#include "simulation/benchmark.h"

extern const sync_strategy_t strategy_futex;

int main(void)
{
    const sync_strategy_t *strategies[] = {
        &strategy_futex
    };
    int thread_counts[] = {1, 2, 4, 8, 16};
    long critical_ns_values[] = {0, 100, 1000, 10000};

    printf("strategy,threads,critical_ns,elapsed_sec,ops_per_sec,correct\n");

    for (size_t s = 0; s < sizeof(strategies)/sizeof(strategies[0]); s++) {
        for (size_t t = 0; t < sizeof(thread_counts)/sizeof(thread_counts[0]); t++) {
            for (size_t c = 0; c < sizeof(critical_ns_values)/sizeof(critical_ns_values[0]); c++) {

                benchmark_config_t cfg = {
                    .num_threads = thread_counts[t],
                    .iters_per_thread = 200000,
                    .critical_section_ns = critical_ns_values[c],
                    .non_critical_ns = 0,
                };

                benchmark_result_t r = run_benchmark(strategies[s], &cfg);

                printf("%s,%d,%ld,%.4f,%.0f,%d\n",
                       strategies[s]->name, cfg.num_threads, cfg.critical_section_ns,
                       r.elapsed_sec, r.ops_per_sec, r.correctness_ok);
            }
        }
    }
    return 0;
}