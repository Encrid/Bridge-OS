/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>

#include "os_types.h"

constexpr uint64_t MILLISECOND = 1'000'000;

os_event_t *os_event_create(void)
{
    os_event_t *e = malloc(sizeof(*e));

    if (!e)
        return (nullptr);
    e->handle = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    if (e->handle)
        return (e);
    free(e);
    return (nullptr);
}

void os_event_free(os_event_t *e)
{
    if (!e)
        return;
    CloseHandle(e->handle);
    free(e);
}

void os_event_set(os_event_t *e)
{
    SetEvent(e->handle);
}

void os_event_wait(os_event_t *e, uint64_t nanos)
{
    uint64_t ms = nanos / MILLISECOND + (nanos % MILLISECOND > 0);

    WaitForSingleObject(e->handle, ms < INFINITE ? (DWORD)ms : INFINITE);
}
