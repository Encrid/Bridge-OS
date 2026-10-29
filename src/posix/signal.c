/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <signal.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "os.h"

static atomic_bool *get_caught(void)
{
    static atomic_bool caught;

    return (&caught);
}

static void catch_stop([[maybe_unused]] int number)
{
    if (atomic_exchange(get_caught(), true))
        _Exit(1);
}

static int catch_one(int number, const struct sigaction *act)
{
    struct sigaction old = {};

    sigaction(number, nullptr, &old);
    if (old.sa_handler == SIG_IGN)
        return (0);
    return (sigaction(number, act, nullptr));
}

int os_signal_catch(void)
{
    struct sigaction act = {};

    act.sa_handler = catch_stop;
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    return (catch_one(SIGINT, &act) | catch_one(SIGTERM, &act)
        | catch_one(SIGHUP, &act) | (signal(SIGPIPE, SIG_IGN) == SIG_ERR));
}

bool os_signal_is_caught(void)
{
    return (atomic_load(get_caught()));
}
