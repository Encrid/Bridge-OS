/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <string.h>

#include "os_watch.h"

static int give_name(os_watch_t *w, const char **name)
{
    *name = w->reading->bytes + w->at;
    w->at += strlen(*name) + 1;
    return (1);
}

static int swap_names(os_watch_t *w)
{
    os_names_t *full = w->filling;

    if (w->ended)
        return (-1);
    w->filling = w->reading;
    w->filling->used = 0;
    w->reading = full;
    w->at = 0;
    if (!w->lost)
        return (0);
    w->lost = false;
    w->reading->used = 0;
    return (2);
}

int os_watch_take(os_watch_t *w, const char **name)
{
    int status = 0;

    if (w->at < w->reading->used)
        return (give_name(w, name));
    os_mutex_lock(w->lock);
    status = swap_names(w);
    os_mutex_unlock(w->lock);
    if (status || !w->reading->used)
        return (status);
    return (give_name(w, name));
}
