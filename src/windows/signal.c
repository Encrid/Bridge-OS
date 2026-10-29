/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdatomic.h>
#include <stdlib.h>

#include "os_types.h"

constexpr DWORD ENDING[] = {
    CTRL_CLOSE_EVENT, CTRL_LOGOFF_EVENT, CTRL_SHUTDOWN_EVENT
};

static atomic_bool *get_caught(void)
{
    static atomic_bool caught;

    return (&caught);
}

static bool is_ending(DWORD event)
{
    for (size_t i = 0; i < sizeof(ENDING) / sizeof(ENDING[0]); i++) {
        if (ENDING[i] == event)
            return (true);
    }
    return (false);
}

static BOOL WINAPI catch_stop(DWORD event)
{
    if (atomic_exchange(get_caught(), true))
        _Exit(1);
    if (is_ending(event))
        Sleep(INFINITE);
    return (TRUE);
}

int os_signal_catch(void)
{
    return (!SetConsoleCtrlHandler(catch_stop, TRUE));
}

bool os_signal_is_caught(void)
{
    return (atomic_load(get_caught()));
}
