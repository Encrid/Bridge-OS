/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <string.h>

#include "os_watch.h"
#include "../common/os_common.h"

static void add_event(os_watch_t *w, const char *path, uint32_t flag)
{
    size_t size = strlen(path) + 1;
    os_names_t *to = w->filling;

    w->ended |= (flag & OS_EVENTS_ENDED) != 0;
    w->lost |= (flag & OS_EVENTS_LOST) != 0;
    if (size <= w->skip + 1 || !os_is_within(path + w->skip, w->depth))
        return;
    size -= w->skip;
    if (to->used + size > OS_NAMES_ROOM) {
        w->lost = true;
        return;
    }
    memcpy(to->bytes + to->used, path + w->skip, size);
    to->used += size;
}

static void on_events([[maybe_unused]] const void *stream, void *info,
    size_t count, void *paths, const uint32_t *flags,
    [[maybe_unused]] const uint64_t *ids)
{
    os_watch_t *w = info;
    char **names = paths;

    os_mutex_lock(w->lock);
    for (size_t i = 0; i < count; i++)
        add_event(w, names[i], flags[i]);
    os_mutex_unlock(w->lock);
}

bool os_open_stream(os_watch_t *w)
{
    size_t len = strlen(w->real);
    const void *real = os_core_string(w);
    const void *paths = nullptr;

    w->skip = len + (w->real[len - 1] != '/');
    if (real) {
        paths = os_core_array(w, real);
        os_core_release(w, real);
    }
    if (paths) {
        w->stream = os_core_create(w, paths, on_events);
        os_core_release(w, paths);
    }
    if (!w->stream)
        return (false);
    os_core_queue(w);
    return (os_core_start(w));
}
