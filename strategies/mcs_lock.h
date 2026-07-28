// based on https://ufdcimages.uflib.ufl.edu/UF/00/09/51/55/00001/199271.pdf

#ifndef MCS_LOCK_H_
#define MCS_LOCK_H_

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

#define MCS_LOCK_INIT 0

typedef struct qnode_t {
    struct qnode_t *_Atomic next;
    _Atomic bool locked;
} qnode_t;

typedef struct qnode_t *_Atomic mcs_lock_t;

void mcs_acquire(mcs_lock_t *L, qnode_t *I);
void mcs_release(mcs_lock_t *L, qnode_t *I);

#endif