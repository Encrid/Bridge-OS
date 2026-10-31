/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>
#include <string.h>

#include "os_watch.h"

static int compare_wd(const void *key, const void *item)
{
    int wd = *(const int *)key;
    int other = ((const os_watched_t *)item)->wd;

    return ((wd > other) - (wd < other));
}

os_watched_t *os_find_folder(const os_watch_t *w, int wd)
{
    return (bsearch(&wd, w->folders, w->count, sizeof(*w->folders),
        compare_wd));
}

static bool is_under(const char *folder, const char *path, size_t len)
{
    return (!strncmp(folder, path, len)
        && (!folder[len] || folder[len] == '/'));
}

void os_drop_tree(os_watch_t *w, const char *path)
{
    size_t len = strlen(path);
    size_t kept = 0;

    for (size_t i = 0; i < w->count; i++) {
        if (is_under(w->folders[i].path, path, len)) {
            inotify_rm_watch(w->fd, w->folders[i].wd);
            free(w->folders[i].path);
            continue;
        }
        w->folders[kept] = w->folders[i];
        kept++;
    }
    w->count = kept;
}

void os_drop_folder(os_watch_t *w, os_watched_t *f)
{
    size_t at = (size_t)(f - w->folders);

    free(f->path);
    memmove(f, f + 1, (w->count - at - 1) * sizeof(*f));
    w->count--;
}
