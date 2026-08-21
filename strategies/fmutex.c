#include "futex.h"
#include "fmutex.h"
#include "sync_interface.h"

#include <stdatomic.h>
#include <stdlib.h>

#define UNLOCKED 0
#define LOCKED_NO_WAIT 1
#define LOCKED_WAIT 2

void fmutex_lock(fmutex_t *m)
{
    uint32_t expected = UNLOCKED;
    // first check - nobody holds the lock
    if (atomic_compare_exchange_strong(m, &expected, LOCKED_NO_WAIT))
    {
        return;
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

void fmutex_unlock(fmutex_t *m)
{
    if (atomic_fetch_sub(m, 1) != LOCKED_NO_WAIT) 
    {
        atomic_store(m, UNLOCKED);
        futex_wake(m, 1);
    }
}

static fmutex_t g_fmutex;

static void fmutex_global_init(void)    { g_fmutex = 0; }
static void fmutex_global_destroy(void) { }
static void *fmutex_ctx_create(void)    { return NULL; }   // nie potrzebuje kontekstu
static void fmutex_ctx_destroy(void *c) { (void)c; }
static void fmutex_lock_adapter(void *c)   { (void)c; fmutex_lock(&g_fmutex); }
static void fmutex_unlock_adapter(void *c) { (void)c; fmutex_unlock(&g_fmutex); }

const sync_strategy_t strategy_futex = {
    .name = "futex",
    .global_init = fmutex_global_init,
    .global_destroy = fmutex_global_destroy,
    .thread_ctx_create = fmutex_ctx_create,
    .thread_ctx_destroy = fmutex_ctx_destroy,
    .lock = fmutex_lock_adapter,
    .unlock = fmutex_unlock_adapter,
};