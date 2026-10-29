/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <time.h>

#include "../posix/os_types.h"

int os_cond_start(pthread_cond_t *cond)
{
    pthread_condattr_t attr;
    int failed;

    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    failed = pthread_cond_init(cond, &attr);
    pthread_condattr_destroy(&attr);
    return (failed);
}

void os_cond_wait(os_event_t *e, uint64_t nanos)
{
    struct timespec end;
    uint64_t rest;
    int late = 0;

    clock_gettime(CLOCK_MONOTONIC, &end);
    rest = (uint64_t)end.tv_nsec + nanos % OS_SECOND;
    end.tv_sec += (time_t)(nanos / OS_SECOND + rest / OS_SECOND);
    end.tv_nsec = (long)(rest % OS_SECOND);
    while (!e->set && !late)
        late = pthread_cond_timedwait(&e->cond, &e->lock, &end);
}
