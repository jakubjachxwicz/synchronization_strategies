#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "simulation/benchmark.h"

extern sync_strategy_t strategy_futex;
extern sync_strategy_t strategy_spin_then_park;
extern sync_strategy_t strategy_mcs_lock;
extern sync_strategy_t strategy_pthread_mutex;
extern sync_strategy_t strategy_pthread_spinlock;

static struct option long_options[] = {
    {"lock",         required_argument, 0, 0},
    {"threads",      required_argument, 0, 1},
    {"iters",        required_argument, 0, 2},
    {"critical",     required_argument, 0, 3},
    {"non_critical", required_argument, 0, 4},
    {0,              0,                 0, 0}
};

int main(int argc, char *argv[])
{
    char* lock = "futex";
    int thread_count = 4, iterations = 100000;
    long critical_section_ns = 10000, non_critical_ns = 10;

    int opt;
    while ((opt = getopt_long(argc, argv, "l:t:i:c:nc:", long_options, NULL)) != -1) {
        switch (opt) {
            case 0: lock = optarg;                      break;
            case 1: thread_count = atoi(optarg);        break;
            case 2: iterations = atoi(optarg);          break;
            case 3: critical_section_ns = atoi(optarg); break;
            case 4: non_critical_ns = atoi(optarg);     break;
            default:
                fprintf(stderr, "Unexpected argument used: %s [--x=<val>] [--y=<val>] [--name=<str>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // TODO: parameters validation and improvements

    sync_strategy_t* strategies[] = {
        NULL,
        // &strategy_pthread_mutex,
        // &strategy_pthread_spinlock
    };
    if (strcmp(lock, "futex") == 0)
    {
        strategies[0] = &strategy_futex;
    } else if (strcmp(lock, "spin_then_park") == 0 || strcmp(lock, "spin_park") == 0)
    {
        strategies[0] = &strategy_spin_then_park;
    } else if (strcmp(lock, "mcs_lock") == 0 || strcmp(lock, "mcs") == 0)
    {
        strategies[0] = &strategy_mcs_lock;
    } else
    {
        fprintf(stderr, "Strategy %s not recognized\n", lock);
        exit(EXIT_FAILURE);
    }

    // printf("strategy,threads,critical_ns,elapsed_sec,ops_per_sec,correct\n");

    benchmark_config_t cfg = {
        .num_threads = thread_count,
        .iters_per_thread = iterations,
        .critical_section_ns = critical_section_ns,
        .non_critical_ns = non_critical_ns,
    };

    size_t n = sizeof(strategies) / sizeof(strategies[0]);
    for (size_t i = 0; i < n; i++)
    {
        sync_strategy_t* strategy = strategies[i];
        
        benchmark_result_t r = run_benchmark(strategy, &cfg);

        printf("%s,%d,%ld,%.4f,%.0f,%.4f,%d\n",
            strategy->name, cfg.num_threads, cfg.critical_section_ns,
            r.elapsed_sec, r.ops_per_sec, r.fairness, r.correctness_ok);
    }
    
    return 0;
}