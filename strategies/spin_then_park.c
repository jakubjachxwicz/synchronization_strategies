#include "futex.h"
#include "spin_then_park.h"
#include "sync_interface.h"

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

static ad_mutex_t g_ad_mutex;

static void ad_mutex_global_init(void)    { g_ad_mutex = AD_MUTEX_INIT; }
static void ad_mutex_global_destroy(void) { }
static void *ad_mutex_ctx_create(void)    { return NULL; }
static void ad_mutex_ctx_destroy(void *c) { (void)c; }
static void ad_mutex_lock_adapter(void *c)   { (void)c; am_lock(&g_ad_mutex); }
static void ad_mutex_unlock_adapter(void *c) { (void)c; am_unlock(&g_ad_mutex); }

const sync_strategy_t strategy_spin_then_park = {
    .name = "spin then park",
    .global_init = ad_mutex_global_init,
    .global_destroy = ad_mutex_global_destroy,
    .thread_ctx_create = ad_mutex_ctx_create,
    .thread_ctx_destroy = ad_mutex_ctx_destroy,
    .lock = ad_mutex_lock_adapter,
    .unlock = ad_mutex_unlock_adapter,
};