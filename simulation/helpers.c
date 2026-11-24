#include "helpers.h"

static double array_sum(long *numbers, int n)
{
    long result = 0;
    for (int i = 0; i < n; i++)
        result += numbers[i];
    return (double)result;
}

static double array_min(long *numbers, int n)
{
    long result = numbers[0];
    for (int i = 1; i < n; i++)
        if (numbers[i] < result)
            result = numbers[i];
    return (double)result;
}

static double array_max(long *numbers, int n)
{
    long result = numbers[0];
    for (int i = 1; i < n; i++)
        if (numbers[i] > result)
            result = numbers[i];
    return (double)result;
}

double calculate_fairness(long *numbers, int n)
{
    double l = (double)n * array_min(numbers, n) / array_sum(numbers, n);
    double r = array_sum(numbers, n) / n / array_max(numbers, n);
    return l < r ? l : r;
}

void busy_wait_ns(long ns)
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

inline long long now_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}