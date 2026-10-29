/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "os.h"

constexpr size_t FRAME = 1024;

static unsigned go_deep(unsigned depth)
{
    volatile unsigned char frame[FRAME];

    for (size_t i = 0; i < FRAME; i++)
        frame[i] = (unsigned char)depth;
    if (!depth)
        return (frame[FRAME - 1]);
    return (go_deep(depth - 1) + frame[FRAME - 1]);
}

static void run_deep(void *arg)
{
    unsigned *depth = arg;

    *depth = go_deep(*depth);
}

int main(int argc, char **argv)
{
    unsigned long long deep;
    os_thread_t *t;
    unsigned depth;

    if (argc != 3)
        return (printf("usage: stack <bytes of the stack> <bytes deep>\n"), 2);
#ifdef _WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
    deep = strtoull(argv[2], nullptr, 10);
    depth = (unsigned)(deep / FRAME);
    t = os_thread_start(run_deep, &depth, strtoull(argv[1], nullptr, 10));
    if (!t)
        return (printf("FAIL: thread\n"), 1);
    os_thread_join(t);
    printf("a call %llu bytes deep returned\n", deep);
    return (0);
}
