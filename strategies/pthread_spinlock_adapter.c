#include <pthread.h>
#include "sync_interface.h"

static pthread_spinlock_t g_pthread_spinlock;

static void pthread_spinlock_adapter_global_init(void)    { pthread_spin_init(&g_pthread_spinlock, PTHREAD_PROCESS_PRIVATE); }
static void pthread_spinlock_adapter_global_destroy(void) { pthread_spin_destroy(&g_pthread_spinlock); }
static void *pthread_spinlock_adapter_ctx_create(void)    { return NULL; }
static void pthread_spinlock_adapter_ctx_destroy(void *c) { (void)c; }
static void pthread_spinlock_lock_adapter(void *c)   { (void)c; pthread_spin_lock(&g_pthread_spinlock); }
static void pthread_spinlock_unlock_adapter(void *c) { (void)c; pthread_spin_unlock(&g_pthread_spinlock); }

const sync_strategy_t strategy_pthread_spinlock = {
    .name = "pthread spinlock",
    .global_init = pthread_spinlock_adapter_global_init,
    .global_destroy = pthread_spinlock_adapter_global_destroy,
    .thread_ctx_create = pthread_spinlock_adapter_ctx_create,
    .thread_ctx_destroy = pthread_spinlock_adapter_ctx_destroy,
    .lock = pthread_spinlock_lock_adapter,
    .unlock = pthread_spinlock_unlock_adapter,
};