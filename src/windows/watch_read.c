/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <wchar.h>

#include "os_types.h"
#include "../common/os_common.h"

constexpr DWORD CHANGES = FILE_NOTIFY_CHANGE_FILE_NAME
    | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE
    | FILE_NOTIFY_CHANGE_SIZE;

bool os_read_changes(os_watch_t *w)
{
    w->reading = ReadDirectoryChangesW(w->folder, w->records,
        OS_CHANGES_ROOM, w->depth > 1, CHANGES, nullptr, &w->changes,
        nullptr);
    return (w->reading);
}

bool os_read_above(os_watch_t *w)
{
    w->looking = ReadDirectoryChangesW(w->above, w->above_records,
        OS_ABOVE_ROOM, false, FILE_NOTIFY_CHANGE_DIR_NAME, nullptr,
        &w->above_changes, nullptr);
    return (w->looking);
}

bool os_is_in_place(os_watch_t *w)
{
    DWORD size = (DWORD)wcslen(w->final) + 1;
    DWORD got = GetFinalPathNameByHandleW(w->folder, w->again, size, 0);

    return (got == size - 1 && !wcscmp(w->again, w->final));
}

int os_finish_read(os_watch_t *w)
{
    DWORD bytes = 0;

    w->reading = false;
    w->stopped = !GetOverlappedResult(w->folder, &w->changes, &bytes, false);
    w->at = 0;
    w->got = w->stopped ? 0 : bytes;
    return (w->got || w->stopped ? OS_AGAIN : 2);
}

void os_end_read(HANDLE handle, OVERLAPPED *overlapped)
{
    DWORD bytes = 0;

    CancelIoEx(handle, overlapped);
    GetOverlappedResult(handle, overlapped, &bytes, true);
}
