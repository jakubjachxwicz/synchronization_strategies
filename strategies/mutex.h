#ifndef MUTEX_H
#define MUTEX_H

#include <stdatomic.h>
#include <stdint.h>

#define MUTEX_INIT 0

typedef _Atomic uint32_t mutex_t;

void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);

#endif