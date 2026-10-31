/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "os_watch.h"
#include "../common/os_common.h"

os_watch_t *os_watch_start(const char *folder, uint32_t depth)
{
    os_watch_t *w = depth ? calloc(1, sizeof(*w)) : nullptr;
    char *whole = nullptr;

    if (!w)
        return (nullptr);
    w->depth = depth;
    w->fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    w->folders = calloc(OS_FOLDERS_ROOM, sizeof(*w->folders));
    w->room = OS_FOLDERS_ROOM;
    whole = os_path_whole(folder);
    if (whole)
        snprintf(w->full, sizeof(w->full), "%s/", whole);
    free(whole);
    w->base = strlen(w->full);
    if (w->fd >= 0 && w->folders && w->base && !os_add_tree(w, w->base, 0))
        return (w);
    os_watch_stop(w);
    return (nullptr);
}

void os_watch_stop(os_watch_t *w)
{
    if (!w)
        return;
    if (w->fd >= 0)
        close(w->fd);
    for (size_t i = 0; i < w->count; i++)
        free(w->folders[i].path);
    free(w->folders);
    free(w);
}

static int step(os_watch_t *w, const char **name)
{
    ssize_t got = 0;

    if (w->at < w->got)
        return (os_use_event(w, name));
    got = read(w->fd, w->events, OS_EVENTS_ROOM);
    w->at = 0;
    w->got = got > 0 ? (size_t)got : 0;
    return (w->got ? OS_AGAIN : 0);
}

int os_watch_take(os_watch_t *w, const char **name)
{
    int status = OS_AGAIN;

    while (status == OS_AGAIN && !w->stopped)
        status = step(w, name);
    return (w->stopped ? -1 : status);
}
