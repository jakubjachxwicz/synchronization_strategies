#include "futex.h"
#include "adaptive_mutex.h"

#include <stdatomic.h>
#include <stdlib.h>

#define UNLOCKED 0
#define LOCKED_NO_WAIT 1
#define LOCKED_WAIT 2

void am_lock(ad_mutex_t *m)
{
    uint32_t expected = UNLOCKED;
    // first check - nobody holds the lock
    if (atomic_compare_exchange_strong(m, &expected, LOCKED_NO_WAIT))
    {
        return;
    }

    // spinning
    for (int i = 0; i < SPIN_COUNT; i++)
    {
        expected = atomic_load(m);

        if (expected == UNLOCKED)
        {
            // lock released during spin
            if (atomic_compare_exchange_strong(m, &expected, LOCKED_NO_WAIT))
            {
                return;
            }
        }
        else if (expected == LOCKED_WAIT)
        {
            break;
        }

        // verify: neccessity of CPU pause after spinning
    }

    if (expected != LOCKED_WAIT)
    {
        expected = atomic_exchange(m, LOCKED_WAIT);
    }

    while (expected != UNLOCKED)
    {
        futex_wait(m, LOCKED_WAIT);
        expected = atomic_exchange(m, LOCKED_WAIT);
    }
}

void am_unlock(ad_mutex_t *m)
{
    if (atomic_fetch_sub(m, 1) != LOCKED_NO_WAIT) 
    {
        atomic_store(m, UNLOCKED);
        futex_wake(m, 1);
    }
}