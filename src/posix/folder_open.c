/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "os_types.h"

constexpr int FOLDER = O_RDONLY | O_DIRECTORY | O_CLOEXEC;

static int step(int at, const char *name, bool last)
{
    int next = last ? openat(at, name, FOLDER) : os_open_search(at, name);

    close(at);
    return (next);
}

static int open_path(const char *path)
{
    size_t size = strlen(path) + 1;
    char *names = malloc(size);
    char *place = nullptr;
    char *name = nullptr;
    char *after = nullptr;
    int at = -1;

    if (names) {
        memcpy(names, path, size);
        at = os_open_search(AT_FDCWD, *path == '/' ? "/" : ".");
        name = strtok_r(names, "/", &place);
    }
    while (name && at >= 0) {
        after = strtok_r(nullptr, "/", &place);
        at = step(at, name, !after);
        name = after;
    }
    free(names);
    return (at);
}

os_folder_t *os_folder_open(const char *path)
{
    os_folder_t *f = calloc(1, sizeof(*f));
    int fd = -1;

    if (!f)
        return (nullptr);
    f->dir = opendir(path);
    if (!f->dir && errno == ENAMETOOLONG)
        fd = open_path(path);
    if (fd >= 0)
        f->dir = fdopendir(fd);
    if (f->dir)
        return (f);
    close(fd);
    free(f);
    return (nullptr);
}

void os_folder_close(os_folder_t *f)
{
    if (!f)
        return;
    closedir(f->dir);
    free(f->bytes);
    free(f);
}
