/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "os.h"

constexpr size_t FRAME = 1024;
constexpr size_t LAST = 4096;
constexpr size_t SLACK = 65536;
constexpr size_t MAIN_MOST = 1 << 30;
constexpr unsigned PAST = 64;
constexpr unsigned TRIES = 4;

struct run {
    size_t size;
    size_t first;
    bool within;
};

static bool past;

static size_t read_left(void)
{
    size_t left = 0;

    for (unsigned i = 0; i < TRIES && !left; i++)
        left = os_thread_stack_left();
    return (left);
}

static unsigned go_deep(unsigned beyond)
{
    volatile unsigned char frame[FRAME];
    bool last = os_thread_stack_left() < LAST;

    for (size_t i = 0; i < FRAME; i++)
        frame[i] = (unsigned char)beyond;
    if (last && !beyond)
        return (frame[0]);
    return (go_deep(beyond - last) + frame[FRAME - 1]);
}

static void run_thread(void *arg)
{
    struct run *run = arg;

    run->first = read_left();
    run->within = run->first <= run->size && run->first + SLACK > run->size;
    if (run->within)
        go_deep(past ? PAST : 0);
}

static int run_one(size_t size)
{
    struct run run = {.size = size};
    os_thread_t *t = os_thread_start(run_thread, &run, size);

    if (!t)
        return (printf("FAIL: thread of %zu bytes\n", size), 1);
    os_thread_join(t);
    if (!run.within)
        return (printf("thread of %zu bytes: %zu bytes left at its start\n",
            size, run.first), 1);
    printf("thread of %zu bytes: within its stack, used down to %zu bytes\n",
        size, LAST);
    return (0);
}

static int run_main(void)
{
    size_t first = read_left();

    if (!first || first > MAIN_MOST)
        return (printf("main: %zu bytes left at its start\n", first), 1);
    go_deep(past ? PAST : 0);
    printf("main: used down to %zu bytes\n", LAST);
    return (0);
}

int main(int argc, char **argv)
{
    int status = 0;

#ifdef _WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
    past = argc > 2 && !strcmp(argv[argc - 1], "past");
    if (argc >= 2 && !strcmp(argv[1], "main"))
        return (run_main());
    if (argc < 3 || strcmp(argv[1], "thread"))
        return (printf("usage: stack_left main|thread <bytes>... [past]\n"),
            2);
    for (int i = 2; i < argc - past && !status; i++)
        status = run_one(strtoull(argv[i], nullptr, 10));
    return (status);
}
