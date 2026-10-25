/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <dlfcn.h>

#include "os_types.h"

void *os_lib_open(const char *path)
{
    return (dlopen(path, RTLD_NOW | RTLD_LOCAL));
}

void *os_lib_get_symbol(void *lib, const char *name)
{
    return (dlsym(lib, name));
}

void os_lib_close(void *lib)
{
    dlclose(lib);
}

int os_lib_pin(const char *path)
{
    void *lib = dlopen(path, RTLD_NOW | RTLD_NOLOAD | RTLD_NODELETE);

    if (!lib)
        return (-1);
    dlclose(lib);
    return (0);
}
