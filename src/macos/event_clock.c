/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "../posix/os_types.h"

int os_cond_start(pthread_cond_t *cond)
{
    return (pthread_cond_init(cond, nullptr));
}

void os_cond_wait(os_event_t *e, uint64_t nanos)
{
    uint64_t start = os_get_now();
    uint64_t gone = 0;
    struct timespec rest;

    while (!e->set && gone < nanos) {
        rest.tv_sec = (time_t)((nanos - gone) / OS_SECOND);
        rest.tv_nsec = (long)((nanos - gone) % OS_SECOND);
        pthread_cond_timedwait_relative_np(&e->cond, &e->lock, &rest);
        gone = os_get_now() - start;
    }
}
