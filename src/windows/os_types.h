/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_OS_TYPES_H
    #define BRIDGE_OS_TYPES_H

    #define WIN32_LEAN_AND_MEAN
    #include <stdatomic.h>
    #include <windows.h>

    #include "os.h"

struct os_thread {
    HANDLE handle;
    os_thread_fn_t fn;
    void *arg;
};

struct os_mutex {
    CRITICAL_SECTION section;
};

struct os_event {
    HANDLE handle;
};

constexpr size_t OS_NAME_ROOM = 256;

struct os_folder {
    HANDLE find;
    WIN32_FIND_DATAW data;
    bool pending;
    wchar_t *whole;
    char *bytes;
    char name[3 * OS_NAME_ROOM];
};

struct os_stop {
    atomic_bool caught;
    const char *line;
    DWORD length;
};

#endif
