/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <string.h>

#include "os_watch.h"
#include "../common/os_common.h"

constexpr uint32_t ENDS = IN_DELETE_SELF | IN_MOVE_SELF | IN_UNMOUNT
    | IN_IGNORED;

static int end_folder(os_watch_t *w, os_watched_t *f, uint32_t mask)
{
    w->stopped = !*f->path;
    if (!w->stopped && (mask & IN_IGNORED))
        os_drop_folder(w, f);
    return (OS_AGAIN);
}

static void follow_folder(os_watch_t *w, uint32_t mask)
{
    if ((mask & (IN_CREATE | IN_MOVED_TO))
        && os_is_within(w->name, w->depth - 1)) {
        snprintf(w->full + w->base, sizeof(w->full) - w->base, "%s",
            w->name);
        w->stopped = os_add_tree(w, strlen(w->full)) != 0;
        w->full[w->base] = '\0';
    }
    if (mask & IN_MOVED_FROM)
        os_drop_tree(w, w->name);
}

static int give_name(os_watch_t *w, const os_watched_t *f,
    const struct inotify_event *e, const char **name)
{
    size_t at = strlen(f->path);

    memcpy(w->name, f->path, at);
    if (at) {
        w->name[at] = '/';
        at++;
    }
    memcpy(w->name + at, e->name, strlen(e->name) + 1);
    if (e->mask & IN_ISDIR)
        follow_folder(w, e->mask);
    *name = w->name;
    return (1);
}

int os_use_event(os_watch_t *w, const char **name)
{
    const struct inotify_event *e = (const void *)(w->events + w->at);
    os_watched_t *f = os_find_folder(w, e->wd);

    w->at += sizeof(*e) + e->len;
    if (e->mask & IN_Q_OVERFLOW) {
        w->stopped = os_add_tree(w, w->base) != 0;
        return (2);
    }
    if (!f)
        return (OS_AGAIN);
    if (e->mask & ENDS)
        return (end_folder(w, f, e->mask));
    return (give_name(w, f, e, name));
}
