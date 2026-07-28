// based on https://ufdcimages.uflib.ufl.edu/UF/00/09/51/55/00001/199271.pdf

#ifndef MCS_LOCK_H_
#define MCS_LOCK_H_

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct qnode {
    struct qnode *_Atomic next;
    _Atomic bool locked;
} qnode;

typedef struct qnode *_Atomic mcs_lock;

void mcs_acquire(mcs_lock *L, qnode *I);
void mcs_release(mcs_lock *L, qnode *I);

#endif