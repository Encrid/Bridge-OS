/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>

#include "os_watch.h"

static bool is_folder(const char *path)
{
    os_folder_t *f = os_folder_open(path);
    bool found = f != nullptr;

    os_folder_close(f);
    return (found);
}

static bool make_parts(os_watch_t *w, const char *folder)
{
    w->real = os_path_whole(folder);
    if (!w->real || !is_folder(w->real))
        return (false);
    w->lock = os_mutex_create();
    if (!w->lock)
        return (false);
    w->queue = os_queue_make();
    return (os_core_load(w) && os_open_stream(w));
}

os_watch_t *os_watch_start(const char *folder, uint32_t depth)
{
    os_watch_t *w = depth ? calloc(1, sizeof(*w)) : nullptr;

    if (!w)
        return (nullptr);
    w->depth = depth;
    w->filling = &w->names[0];
    w->reading = &w->names[1];
    if (make_parts(w, folder))
        return (w);
    os_watch_stop(w);
    return (nullptr);
}

void os_watch_stop(os_watch_t *w)
{
    if (!w)
        return;
    if (w->stream)
        os_core_end(w);
    if (w->queue)
        os_queue_free(w->queue);
    if (w->lib)
        os_lib_close(w->lib);
    os_mutex_free(w->lock);
    free(w->real);
    free(w);
}
