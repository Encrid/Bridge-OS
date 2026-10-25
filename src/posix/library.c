/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <dlfcn.h>
#include <pthread.h>

#include "os_types.h"

constexpr unsigned OS_PINS = 64;

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

static bool held(void *const *libs, unsigned used, const void *lib)
{
    for (unsigned i = 0; i < used; i++) {
        if (libs[i] == lib)
            return (true);
    }
    return (false);
}

int os_lib_pin(const char *path)
{
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    static void *libs[OS_PINS];
    static unsigned used;
    void *lib = dlopen(path, RTLD_NOW | RTLD_NOLOAD | RTLD_NODELETE);
    bool kept;

    if (!lib)
        return (-1);
    pthread_mutex_lock(&lock);
    if (held(libs, used, lib) || used == OS_PINS)
        dlclose(lib);
    else
        libs[used++] = lib;
    kept = held(libs, used, lib);
    pthread_mutex_unlock(&lock);
    return (kept ? 0 : -1);
}
