#ifndef SYNC_IFACE_H
#define SYNC_IFACE_H

typedef struct sync_strategy {
    const char *name;

    void (*global_init)(void);
    void (*global_destroy)(void);

    void *(*thread_ctx_create)(void);
    void (*thread_ctx_destroy)(void *ctx);

    void (*lock)(void *ctx);
    void (*unlock)(void *ctx);
} sync_strategy_t;

#endif