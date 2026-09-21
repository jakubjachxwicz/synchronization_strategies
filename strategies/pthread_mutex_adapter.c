#include <pthread.h>
#include "sync_interface.h"

static pthread_mutex_t g_pthread_mutex;

static void pthread_mutex_adapter_global_init(void)    { g_pthread_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; }
static void pthread_mutex_adapter_global_destroy(void) { pthread_mutex_destroy(&g_pthread_mutex); }
static void *pthread_mutex_adapter_ctx_create(void)    { return NULL; }
static void pthread_mutex_adapter_ctx_destroy(void *c) { (void)c; }
static void pthread_mutex_lock_adapter(void *c)   { (void)c; pthread_mutex_lock(&g_pthread_mutex); }
static void pthread_mutex_unlock_adapter(void *c) { (void)c; pthread_mutex_unlock(&g_pthread_mutex); }

const sync_strategy_t strategy_pthread_mutex = {
    .name = "pthread mutex",
    .global_init = pthread_mutex_adapter_global_init,
    .global_destroy = pthread_mutex_adapter_global_destroy,
    .thread_ctx_create = pthread_mutex_adapter_ctx_create,
    .thread_ctx_destroy = pthread_mutex_adapter_ctx_destroy,
    .lock = pthread_mutex_lock_adapter,
    .unlock = pthread_mutex_unlock_adapter,
};