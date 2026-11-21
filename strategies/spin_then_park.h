#ifndef SPIN_THEN_PARK_H
#define SPIN_THEN_PARK_H

#include <stdatomic.h>
#include <stdint.h>

#define AD_MUTEX_INIT 0

#define SPIN_COUNT 20000

typedef _Atomic uint32_t ad_mutex_t;

void am_lock(ad_mutex_t* m);
void am_unlock(ad_mutex_t* m);

#endif