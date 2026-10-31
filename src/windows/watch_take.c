/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <string.h>

#include "os_types.h"
#include "../common/os_common.h"

static bool keep_above(os_watch_t *w)
{
    if (!os_is_in_place(w))
        return (false);
    return (os_read_above(w));
}

static void see_above(os_watch_t *w)
{
    if (w->stopped || !w->looking
        || !HasOverlappedIoCompleted(&w->above_changes))
        return;
    w->looking = false;
    w->stopped = !keep_above(w);
}

static int hand_out(os_watch_t *w, const char **name)
{
    const FILE_NOTIFY_INFORMATION *r = (const void *)(w->records + w->at);
    int units = (int)(r->FileNameLength / sizeof(r->FileName[0]));
    int size = WideCharToMultiByte(CP_UTF8, 0, r->FileName, units, w->name,
        (int)OS_NAME_BYTES - 1, nullptr, nullptr);

    w->at = r->NextEntryOffset ? w->at + r->NextEntryOffset : w->got;
    w->name[size] = '\0';
    for (char *c = strchr(w->name, '\\'); c; c = strchr(c, '\\'))
        *c = '/';
    *name = w->name;
    return (os_is_within(w->name, w->depth) ? 1 : OS_AGAIN);
}

static int step(os_watch_t *w, const char **name)
{
    if (w->at < w->got)
        return (hand_out(w, name));
    if (!w->reading) {
        w->stopped = !os_read_changes(w);
        return (OS_AGAIN);
    }
    if (!HasOverlappedIoCompleted(&w->changes))
        return (0);
    return (os_finish_read(w));
}

int os_watch_take(os_watch_t *w, const char **name)
{
    int status = OS_AGAIN;

    while (status == OS_AGAIN && !w->stopped)
        status = step(w, name);
    see_above(w);
    return (w->stopped ? -1 : status);
}
