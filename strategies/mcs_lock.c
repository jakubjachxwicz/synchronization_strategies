#include "mcs_lock.h"

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