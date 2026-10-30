/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "os_types.h"

static void *run_thread(void *p)
{
    os_thread_t *t = p;

    t->fn(t->arg);
    return (nullptr);
}

os_thread_t *os_thread_start(os_thread_fn_t fn, void *arg, size_t stack)
{
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    size_t least = stack < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : stack;
    os_thread_t *t = malloc(sizeof(*t));
    pthread_attr_t attr;
    int failed;

    if (!t)
        return (nullptr);
    t->fn = fn;
    t->arg = arg;
    pthread_attr_init(&attr);
    failed = pthread_attr_setstacksize(&attr, (least + page - 1) / page * page)
        || pthread_create(&t->handle, &attr, run_thread, t);
    pthread_attr_destroy(&attr);
    if (!failed)
        return (t);
    free(t);
    return (nullptr);
}

void os_thread_join(os_thread_t *t)
{
    pthread_join(t->handle, nullptr);
    free(t);
}

uint64_t os_get_thread_id(void)
{
    return ((uint64_t)(uintptr_t)pthread_self());
}

uint64_t os_get_now(void)
{
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return ((uint64_t)t.tv_sec * OS_SECOND + (uint64_t)t.tv_nsec);
}
