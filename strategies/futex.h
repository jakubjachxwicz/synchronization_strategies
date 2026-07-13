#ifndef FUTEX_H_
#define FUTEX_H_

#include <stdint.h>
#include <stdatomic.h>

typedef _Atomic uint32_t futex_t;

void futex_wait(futex_t *addr, int expected);

void futex_wake(futex_t *addr, int n);

#endif