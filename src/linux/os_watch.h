/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_OS_WATCH_H
    #define BRIDGE_OS_WATCH_H

    #include <limits.h>
    #include <sys/inotify.h>

    #include "../posix/os_types.h"

constexpr uint32_t OS_WATCH_MASK = IN_CREATE | IN_DELETE | IN_MODIFY
    | IN_CLOSE_WRITE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF
    | IN_MOVE_SELF | IN_ONLYDIR;
constexpr size_t OS_EVENTS_ROOM = 65536;
constexpr size_t OS_FOLDERS_ROOM = 16;

typedef struct os_watched {
    int wd;
    char *path;
} os_watched_t;

struct os_watch {
    int fd;
    uint32_t depth;
    bool stopped;
    os_watched_t *folders;
    size_t count;
    size_t room;
    size_t at;
    size_t got;
    size_t base;
    char full[PATH_MAX + 1];
    char name[PATH_MAX + NAME_MAX + 2];
    alignas(struct inotify_event) char events[OS_EVENTS_ROOM];
};

int os_add_tree(os_watch_t *w, size_t end);

os_watched_t *os_find_folder(const os_watch_t *w, int wd);

void os_drop_tree(os_watch_t *w, const char *path);

void os_drop_folder(os_watch_t *w, os_watched_t *f);

int os_use_event(os_watch_t *w, const char **name);

#endif
