#include "helpers.h"


// inline void busy_wait_ns(long nanoseconds)
// {
//     struct timespec start, now;
//     clock_gettime(CLOCK_MONOTONIC, &start);

//     long elapsed;
//     do {
//         clock_gettime(CLOCK_MONOTONIC, &now);
//         elapsed = (now.tv_sec - start.tv_sec) * 1000000000L
//                 + (now.tv_nsec - start.tv_nsec);
//     } while (elapsed < nanoseconds);
// }

double array_sum(long *numbers, int n)
{
    long result = 0;
    for (int i = 0; i < n; i++)
        result += numbers[i];
    return (double)result;
}

double array_min(long *numbers, int n)
{
    long result = numbers[0];
    for (int i = 1; i < n; i++)
        if (numbers[i] < result)
            result = numbers[i];
    return (double)result;
}

double array_max(long *numbers, int n)
{
    long result = numbers[0];
    for (int i = 1; i < n; i++)
        if (numbers[i] > result)
            result = numbers[i];
    return (double)result;
}