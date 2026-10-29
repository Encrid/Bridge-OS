/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef _WIN32
    #define _GNU_SOURCE
    #include <dlfcn.h>
    #include <time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os.h"

static long long step;

#ifndef _WIN32
int clock_gettime(clockid_t clock, struct timespec *now)
{
    void *found = dlsym(RTLD_NEXT, "clock_gettime");
    int (*read_clock)(clockid_t, struct timespec *);
    int result;

    memcpy(&read_clock, &found, sizeof(read_clock));
    result = read_clock(clock, now);
    if (clock == CLOCK_REALTIME)
        now->tv_sec -= step;
    return (result);
}
#endif

int main(int argc, char **argv)
{
    os_event_t *e = os_event_create();
    uint64_t nanos;
    uint64_t spent;

    if (argc < 3 || argc > 4 || !e)
        return (printf("usage: wait <nanoseconds> <seconds back> [set]\n"), 2);
    nanos = strtoull(argv[1], nullptr, 10);
    step = strtoll(argv[2], nullptr, 10);
    if (argc == 4)
        os_event_set(e);
    spent = os_get_now();
    os_event_wait(e, nanos);
    spent = os_get_now() - spent;
    printf("the wait took %s %llu ns\n", spent < nanos ? "less than" : "at least",
        (unsigned long long)nanos);
    os_event_free(e);
    return (0);
}
