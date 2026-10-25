/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <wchar.h>

#include "os_types.h"

static bool widened(const char *path, wchar_t *full, bool *bare)
{
    wchar_t wide[MAX_PATH];
    DWORD len;

    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wide, MAX_PATH))
        return (false);
    *bare = !wcschr(wide, '/') && !wcschr(wide, '\\');
    if (*bare)
        return (wcscpy(full, wide) == full);
    len = GetFullPathNameW(wide, MAX_PATH, full, nullptr);
    return (len && len < MAX_PATH);
}

void *os_lib_open(const char *path)
{
    wchar_t full[MAX_PATH];
    DWORD beside = LOAD_WITH_ALTERED_SEARCH_PATH;
    bool bare;

    if (!widened(path, full, &bare))
        return (nullptr);
    if (bare)
        return ((void *)LoadLibraryW(full));
    return ((void *)LoadLibraryExW(full, nullptr, beside));
}

void *os_lib_get_symbol(void *lib, const char *name)
{
    return ((void *)GetProcAddress((HMODULE)lib, name));
}

void os_lib_close(void *lib)
{
    FreeLibrary((HMODULE)lib);
}

int os_lib_pin(const char *path)
{
    wchar_t full[MAX_PATH];
    HMODULE lib;
    bool bare;

    if (!widened(path, full, &bare))
        return (-1);
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, full, &lib))
        return (-1);
    return (0);
}
