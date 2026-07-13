#include "futex.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdatomic.h>

void futex_wait(futex_t *addr, int expected)
{
    syscall(SYS_futex, addr, FUTEX_WAIT, expected, NULL, NULL, 0);
}

void futex_wake(futex_t *addr, int n)
{
    syscall(SYS_futex, addr, FUTEX_WAKE, n, NULL, NULL, 0);
}