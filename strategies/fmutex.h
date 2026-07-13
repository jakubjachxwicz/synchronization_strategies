#ifndef FMUTEX_H
#define FMUTEX_H

#include <stdatomic.h>
#include <stdint.h>

#define FMUTEX_INIT 0

typedef _Atomic uint32_t fmutex_t;

void fmutex_lock(fmutex_t *m);
void fmutex_unlock(fmutex_t *m);

#endif