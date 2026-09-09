#include "mcs_lock.h"
#include "sync_interface.h"

void mcs_acquire(mcs_lock_t *L, qnode_t *I)
{
    atomic_store(&I->next, NULL);

    qnode_t *pred = atomic_exchange(L, I);
    if (pred != NULL)
    {
        atomic_store(&I->locked, true);
        atomic_store(&pred->next, I);
        
        // spinning
        while (atomic_load(&I->locked)) { }
    }  
}

void mcs_release(mcs_lock_t *L, qnode_t *I) 
{
    qnode_t *succ = atomic_load(&I->next);

    if (succ == NULL)
    {
        qnode_t *expected = I;
        if (atomic_compare_exchange_strong(L, &expected, NULL))
            return;

        do {
            succ = atomic_load(&I->next);
        } while (succ == NULL);
    }

    atomic_store(&succ->locked, false);
}

static mcs_lock_t g_mcs_lock;

static void mcs_global_init(void)    { g_mcs_lock = MCS_LOCK_INIT; }
static void mcs_global_destroy(void) { }
static void *mcs_ctx_create(void)    { return calloc(1, sizeof(qnode_t)); }
static void mcs_ctx_destroy(void *c) { free(c); }
static void mcs_lock_adapter(void *c)   { mcs_acquire(&g_mcs_lock, (qnode_t *)c); }
static void mcs_unlock_adapter(void *c) { mcs_release(&g_mcs_lock, (qnode_t *)c); }

const sync_strategy_t strategy_mcs_lock = {
    .name = "mcs lock",
    .global_init = mcs_global_init,
    .global_destroy = mcs_global_destroy,
    .thread_ctx_create = mcs_ctx_create,
    .thread_ctx_destroy = mcs_ctx_destroy,
    .lock = mcs_lock_adapter,
    .unlock = mcs_unlock_adapter,
};