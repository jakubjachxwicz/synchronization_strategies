#include "futex.h"
#include "mutex.h"
#include "sync_interface.h"

#include <stdatomic.h>
#include <stdlib.h>

#define UNLOCKED 0
#define LOCKED_NO_WAIT 1
#define LOCKED_WAIT 2

void mutex_lock(mutex_t *m)
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

void mutex_unlock(mutex_t *m)
{
    if (atomic_fetch_sub(m, 1) != LOCKED_NO_WAIT) 
    {
        atomic_store(m, UNLOCKED);
        futex_wake(m, 1);
    }
}

static mutex_t g_mutex;

static void mutex_global_init(void)    { g_mutex = MUTEX_INIT; }
static void mutex_global_destroy(void) { }
static void *mutex_ctx_create(void)    { return NULL; }
static void mutex_ctx_destroy(void *c) { (void)c; }
static void mutex_lock_adapter(void *c)   { (void)c; mutex_lock(&g_mutex); }
static void mutex_unlock_adapter(void *c) { (void)c; mutex_unlock(&g_mutex); }

const sync_strategy_t strategy_mutex = {
    .name = "mutex",
    .global_init = mutex_global_init,
    .global_destroy = mutex_global_destroy,
    .thread_ctx_create = mutex_ctx_create,
    .thread_ctx_destroy = mutex_ctx_destroy,
    .lock = mutex_lock_adapter,
    .unlock = mutex_unlock_adapter,
};