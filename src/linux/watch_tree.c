/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os_watch.h"

static bool grow_table(os_watch_t *w)
{
    size_t room = 2 * w->room;
    os_watched_t *more = nullptr;

    if (w->count < w->room)
        return (true);
    more = realloc(w->folders, room * sizeof(*more));
    if (!more)
        return (false);
    w->folders = more;
    w->room = room;
    return (true);
}

static bool keep_folder(os_watch_t *w, int wd, uint32_t parts)
{
    os_watched_t *f = os_find_folder(w, wd);
    size_t size = strlen(w->full + w->base) + 1;
    char *path = malloc(size);

    if (!path || (!f && !grow_table(w))) {
        free(path);
        return (false);
    }
    memcpy(path, w->full + w->base, size);
    if (f) {
        free(f->path);
    } else {
        f = &w->folders[w->count];
        w->count++;
    }
    *f = (os_watched_t){ .wd = wd, .parts = parts, .path = path };
    return (true);
}

static int add_folders(os_watch_t *w, os_folder_t *in, size_t end,
    uint32_t parts)
{
    const char *between = end > w->base ? "/" : "";
    bool is_folder = false;
    int status = 0;

    for (const char *name = os_folder_read_entry(in, &is_folder);
        name && !status; name = os_folder_read_entry(in, &is_folder)) {
        if (!is_folder)
            continue;
        snprintf(w->full + end, sizeof(w->full) - end, "%s%s", between,
            name);
        status = os_add_tree(w, strlen(w->full), parts);
        w->full[end] = '\0';
    }
    return (status);
}

int os_add_tree(os_watch_t *w, size_t end, uint32_t parts)
{
    int wd = inotify_add_watch(w->fd, w->full, OS_WATCH_MASK);
    int status = wd < 0 || !keep_folder(w, wd, parts) ? -1 : 0;
    os_folder_t *in = nullptr;

    if (!status && parts + 1 < w->depth) {
        in = os_folder_open(w->full);
        status = in ? add_folders(w, in, end, parts + 1) : -1;
        os_folder_close(in);
    }
    return (status);
}
