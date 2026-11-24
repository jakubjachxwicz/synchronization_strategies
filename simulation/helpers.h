#ifndef HELPERS_H
#define HELPERS_H

#include <time.h>

double calculate_fairness(long *numbers, int n);
void busy_wait_ns(long ns);
long long now_ns();

#endif