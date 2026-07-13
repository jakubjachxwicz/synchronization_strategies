#include <stdio.h>

#include "strategies/futex.h"

static futex_t futex;

int main() 
{
    printf("Hello, World!\n");

    futex_wait(&futex, 0);

    printf("Hello again!\n");

    return 0;
}