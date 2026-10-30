/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "os_types.h"

static struct os_stop *get_stop(void)
{
    static struct os_stop stop;

    return (&stop);
}

static void catch_stop([[maybe_unused]] int number)
{
    struct os_stop *stop = get_stop();
    [[maybe_unused]] ssize_t written;

    if (atomic_exchange(&stop->caught, true))
        _Exit(1);
    written = write(STDOUT_FILENO, atomic_load(&stop->line),
        atomic_load(&stop->length));
}

static int catch_one(int number, const struct sigaction *act)
{
    struct sigaction old = {};

    sigaction(number, nullptr, &old);
    if (old.sa_handler == SIG_IGN)
        return (0);
    return (sigaction(number, act, nullptr));
}

int os_signal_catch(const char *line)
{
    struct os_stop *stop = get_stop();
    struct sigaction act = {};

    atomic_store(&stop->line, line);
    atomic_store(&stop->length, strlen(line));
    act.sa_handler = catch_stop;
    act.sa_flags = SA_RESTART | SA_NODEFER;
    sigemptyset(&act.sa_mask);
    return (catch_one(SIGINT, &act) | catch_one(SIGTERM, &act)
        | catch_one(SIGHUP, &act) | (signal(SIGPIPE, SIG_IGN) == SIG_ERR));
}

bool os_signal_is_caught(void)
{
    return (atomic_load(&get_stop()->caught));
}
