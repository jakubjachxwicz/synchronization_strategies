#include "helpers.h"


inline void busy_wait_ns(long nanoseconds)
{
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    long elapsed;
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
        elapsed = (now.tv_sec - start.tv_sec) * 1000000000L
                + (now.tv_nsec - start.tv_nsec);
    } while (elapsed < nanoseconds);
}