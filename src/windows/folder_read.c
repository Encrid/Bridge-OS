/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <sys/stat.h>
#include <wchar.h>

#include "os_types.h"
#include "../common/os_common.h"

const char *os_folder_read_entry(os_folder_t *f, bool *is_folder)
{
    const wchar_t *name = f->data.cFileName;

    while (f->pending || FindNextFileW(f->find, &f->data)) {
        f->pending = false;
        if (wcscmp(name, L".") && wcscmp(name, L"..")) {
            WideCharToMultiByte(CP_UTF8, 0, name, -1, f->name,
                sizeof(f->name), nullptr, nullptr);
            *is_folder = f->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            return (f->name);
        }
    }
    return (nullptr);
}

char *os_folder_read_file(os_folder_t *f, const char *name, size_t *len)
{
    size_t at = wcslen(f->whole);
    struct _stat64 st = {};
    FILE *file = nullptr;
    char *bytes = nullptr;

    f->whole[at] = L'\\';
    if (MultiByteToWideChar(CP_UTF8, 0, name, -1, f->whole + at + 1,
        (int)OS_NAME_ROOM))
        file = _wfopen(f->whole, L"rb");
    f->whole[at] = L'\0';
    if (!file)
        return (nullptr);
    _fstat64(_fileno(file), &st);
    bytes = os_read_stream(file, (size_t)st.st_size, &f->bytes, len);
    fclose(file);
    return (bytes);
}
